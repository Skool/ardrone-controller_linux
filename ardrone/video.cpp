// -------------------------------------------------------------------------
// CV Drone (= OpenCV + AR.Drone)
// Copyright(C) 2013 puku0x
// https://github.com/puku0x/cvdrone
//
// This source file is part of CV Drone library.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of EITHER:
// (1) The GNU Lesser General Public License as published by the Free
//     Software Foundation; either version 2.1 of the License, or (at
//     your option) any later version. The text of the GNU Lesser
//     General Public License is included with this library in the
//     file cvdrone-license-LGPL.txt.
// (2) The BSD-style license that is included with this library in
//     the file cvdrone-license-BSD.txt.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files
// cvdrone-license-LGPL.txt and cvdrone-license-BSD.txt for more details.
// -------------------------------------------------------------------------

#include "ardrone.h"
#include "uvlc.h"

// The codes of decoding H.264 video is based on following sites.
// - An ffmpeg and SDL Tutorial - Tutorial 01: Making Screencaps -
//   http://dranger.com/ffmpeg/tutorial01.html
// - AR.Drone Development - 2.1.2 AR.Drone 2.0 Video Decording: FFMPEG + SDL2.0 -
//   http://ardrone-ailab-u-tokyo.blogspot.jp/2012/07/212-ardrone-20-video-decording-ffmpeg.html

// --------------------------------------------------------------------------
// ARDrone::initVideo()
// Description  : Initialize video.
// Return value : SUCCESS: 1  FAILURE: 0
// --------------------------------------------------------------------------
int ARDrone::initVideo(void)
{
	DoLog("Initialize video");

    // AR.Drone 2.0
    if (version.major == ARDRONE_VERSION_2) 
	{
        // Open the IP address and port
        char filename[256];
        sprintf(filename, "tcp://%s:%d", ip, ARDRONE_VIDEO_PORT);
        if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) < 0) 
		{
            CVDRONE_ERROR("avformat_open_input() was failed. (%s, %d)\n", __FILE__, __LINE__);
            return 0;
        }

        // Retrive and dump stream information
        avformat_find_stream_info(pFormatCtx, NULL);
        av_dump_format(pFormatCtx, 0, filename, 0);

        // Find the decoder for the video stream
        pCodecCtx = pFormatCtx->streams[0]->codec;
        AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
        if (pCodec == NULL) 
		{
            CVDRONE_ERROR("avcodec_find_decoder() was failed. (%s, %d)\n", __FILE__, __LINE__);
            return 0;
        }

        // Open codec
        if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) 
		{
            CVDRONE_ERROR("avcodec_open2() was failed. (%s, %d)\n", __FILE__, __LINE__);
            return 0;
        }

        // Allocate video frames and a buffer
        if(NULL == (pFrame = avcodec_alloc_frame()))
		{
			DoLog("Function avcodec_alloc_frame for pFrame failed !", MSG_ERROR);
			return 0;
		}
        if(NULL == (pFrameBGR = avcodec_alloc_frame()))
		{
			DoLog("Function avcodec_alloc_frame for pFrameBGR failed !", MSG_ERROR);
			return 0;
		}
        if(NULL == (bufferBGR = (uint8_t*)av_mallocz(avpicture_get_size(PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height) * sizeof(uint8_t))))
		{
			DoLog("Function av_mallocz failed !", MSG_ERROR);
			return 0;
		}

        // Assign appropriate parts of buffer to image planes in pFrameBGR
        avpicture_fill((AVPicture*)pFrameBGR, bufferBGR, PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

        // Convert it to BGR
        if(NULL == (pConvertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_BGR24, SWS_SPLINE, NULL, NULL, NULL)))
		{
			DoLog("Function sws_getContext failed !", MSG_ERROR);
			return 0;
		}
    }    
    else // AR.Drone 1.0
	{
        // Open the IP address and port
        if (!sockVideo.open(ip, ARDRONE_VIDEO_PORT))
		{
            CVDRONE_ERROR("UDPSocket::open(port=%d) was failed. (%s, %d)\n", ARDRONE_VIDEO_PORT, __FILE__, __LINE__);
            return 0;
        }

        // Set codec
        pCodecCtx = avcodec_alloc_context();
        pCodecCtx->width = 320;
        pCodecCtx->height = 240;

        // Allocate a buffer
        bufferBGR = (uint8_t*)av_mallocz(avpicture_get_size(PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height));
    }

    // Allocate an IplImage
    img = cvCreateImage(cvSize(pCodecCtx->width, (pCodecCtx->height == 368) ? 360 : pCodecCtx->height), IPL_DEPTH_8U, 3);
    if (!img)
	{
        CVDRONE_ERROR("cvCreateImage() was failed. (%s, %d)\n", __FILE__, __LINE__);
        return 0;
    }

    // Clear the image
    cvZero(img);	

    // Enable thread loop
    bflagVideo = true;

    // Create a thread
	UINT id;
    threadVideo = (HANDLE)_beginthreadex(NULL, 0, runVideo, this, 0, &id);
    if (threadVideo == INVALID_HANDLE_VALUE)
	{
        CVDRONE_ERROR("_beginthreadex() was failed. (%s, %d)\n", __FILE__, __LINE__);
        return 0;
    }

	DoLog("Video initialized");

    return 1;
}

// --------------------------------------------------------------------------
// ARDrone::loopVideo()
// Description  : Thread function.
// Return value : SUCCESS:0
// --------------------------------------------------------------------------
UINT ARDrone::loopVideo(void)
{
    while(bflagVideo)
	{
        // Get video stream
        if (!getVideo())
		{
			bNeedVideoRestart = true;
			break;
		}        

		// Set the update time		
		CSVideoWD.Enter();
		lVideoUpdateTime = m_pWatch->Time();
		CSVideoWD.Leave();		

		Sleep(1);
    }

    // Disable thread loop
    bflagVideo = false;

	return 0;
}

// --------------------------------------------------------------------------
// ARDrone::getVideo()
// Description  : Get AR.Drone's video stream.
// Return value : SUCCESS: 1  FAILURE: 0
// --------------------------------------------------------------------------
int ARDrone::getVideo(void)
{
    // AR.Drone 2.0
    if (version.major == ARDRONE_VERSION_2)
	{
        AVPacket packet;
        int frameFinished = 0;

		// Return codes
		int iResDecode = 0;
		int iResScale = 0;

        // Read all frames
        while(av_read_frame(pFormatCtx, &packet) >= 0)
		{
            // Decode the frame
            iResDecode = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
			if(iResDecode < 0)
			{
				DoLog(wxString::Format("avcodec_decode_video2 failed with error %d (0x%X), current frame will be ignored.", iResDecode, iResDecode), MSG_ERROR);
			}          
            
			if (frameFinished)
			{
                LockVideo();
                
				// Ugly hack: The application sometimes crashes, because the data of the incoming packet are corrupted, but only if usb recording is active
				// As i don't know why this happens, and how the bad packets can be detected, ignore packets were image size in not as expected.
				// TODO: check how to detect bad packets or format using an FFmpeg function
				if( (pFrame->width == 640) || (pFrame->width == 1280) )
				{
					// Convert to BGR
					iResScale = sws_scale(pConvertCtx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameBGR->data, pFrameBGR->linesize);
					if(iResScale <= 0)
					{
						DoLog(wxString::Format("sws_scale result is %d (0x%X) !", iResScale, iResScale), MSG_WARNING);
					}
				}
				else
				{
					DoLog(wxString::Format("Catched unexpected Frame size (width: %d, height: %d), this frame must be ignored !", pCodecCtx->width, pCodecCtx->height), MSG_ERROR);
				}

				UnlockVideo();

                // Free the packet and return immediately
                av_free_packet(&packet);
                return 1;
            }

            // Free the packet
            av_free_packet(&packet);
        }

		DoLog("Failed to get video Frame, will break video thread !", MSG_ERROR);
		return 0;
    }
    // AR.Drone 1.0
    else 
	{
        // Send request
        sockVideo.sendf("\x01\x00\x00\x00");

        // Receive data
        uint8_t buf[122880];
        int size = sockVideo.receive((void*)&buf, sizeof(buf));

        // Received something
        if (size > 0) 
		{
            LockVideo();

            // Decode UVLC video
            UVLC::DecodeVideo(buf, size, bufferBGR, &pCodecCtx->width, &pCodecCtx->height);

            UnlockVideo();
        }
    }

    return 1;
}

// --------------------------------------------------------------------------
// ARDrone::getImage()
// Description  : Get an image from the AR.Drone's camera.
// Return value : Pointer to an IplImage (OpenCV image)
// --------------------------------------------------------------------------
IplImage* ARDrone::getImage(void)
{
    // There is no image
	if (!img || !bflagVideo) 
	{
		return NULL;
	}

	LockVideo();

    // AR.Drone 2.0
    if (version.major == ARDRONE_VERSION_2)
	{
		// Invalid frames from GetVideo must also be ignored here
		if( (pCodecCtx->width == 640) || (pCodecCtx->width == 1280) )
		{
			// Copy the frame to the IplImage
			memcpy(img->imageData, pFrameBGR->data[0], pCodecCtx->width * ((pCodecCtx->height == 368) ? 360 : pCodecCtx->height) * sizeof(uint8_t) * 3);
		}
    }
    // AR.Drone 1.0
    else
	{
        // If the sizes of buffer and IplImage are differnt
        if (pCodecCtx->width != img->width || pCodecCtx->height != img->height) 
		{
            // Resize the image to 320x240
            IplImage *small_img = cvCreateImageHeader(cvSize(pCodecCtx->width, pCodecCtx->height), IPL_DEPTH_8U, 3);
            small_img->imageData = (char*)bufferBGR;
            cvResize(small_img, img, CV_INTER_CUBIC);
            cvReleaseImageHeader(&small_img);
        }
        else 
		{
			// For 320x240 image, just copy it
			memcpy(img->imageData, bufferBGR, pCodecCtx->width * pCodecCtx->height * sizeof(uint8_t) * 3);
		}
    }

	UnlockVideo();

    return img;
}

// --------------------------------------------------------------------------
// ARDrone::finalizeVideo()
// Description  : Finalize video.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::finalizeVideo(void)
{
    // Disable thread loop
    bflagVideo = false;

	DoLog("Finalize video");

    // Destroy the thread
    if (threadVideo != INVALID_HANDLE_VALUE) 
	{
        //WaitForSingleObject(threadVideo, INFINITE);
		DWORD dwRes = WaitForSingleObject(threadVideo, 2500);
		switch(dwRes)
		{
			case WAIT_TIMEOUT:
				DoLog("Wait for video: thread timed out !", MSG_ERROR);
				TerminateThread(threadVideo, -1);
				break;

			case WAIT_FAILED:
				DoLog("Wait for video: thread failed !", MSG_ERROR);
				TerminateThread(threadVideo, -1);
				break;

			default:
				break;
		}

        if(FALSE == CloseHandle(threadVideo))
		{
			DoLog("Failed to close video thread handle", MSG_ERROR);
		}
        threadVideo = INVALID_HANDLE_VALUE;
    }

    // Release the IplImage
    if(img)
	{
        cvReleaseImage(&img);
        img = NULL;
    }

    // AR.Drone 2.0
    if (version.major == ARDRONE_VERSION_2) 
	{
        // Deallocate the frame
        if (pFrame)
		{
            av_free(pFrame);
            pFrame = NULL;
        }

        // Deallocate the frame
        if (pFrameBGR)
		{
            av_free(pFrameBGR);
            pFrameBGR = NULL;
        }

        // Deallocate the buffer
        if (bufferBGR)
		{
            av_free(bufferBGR);
            bufferBGR = NULL;
        }

        // Deallocate the convert context
        if (pConvertCtx)
		{
            sws_freeContext(pConvertCtx);
            pConvertCtx = NULL;
        }

        // Deallocate the codec
        if (pCodecCtx)
		{
            avcodec_close(pCodecCtx);
            pCodecCtx = NULL;
        }

        // Deallocate the format context
        if (pFormatCtx) 
		{
            avformat_close_input(&pFormatCtx);
            pFormatCtx = NULL;
        }
    }
    // AR.Drone 1.0
    else 
	{
        // Deallocate the buffer
        if (bufferBGR) 
		{
            av_free(bufferBGR);
            bufferBGR = NULL;
        }

        // Deallocate the codec
        if (pCodecCtx) 
		{
            avcodec_close(pCodecCtx);
            pCodecCtx = NULL;
        }

        // Close the socket
        sockVideo.close();
    }

	DoLog("Video finalized");
}