


/*
 * Copyright 2009-2015 Freescale Semiconductor, Inc. All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*
 * @file mx6s_v4l2_capture.c
 *
 * @brief MX6sl/sx Video For Linux 2 driver test application
 *
 */

#ifdef __cplusplus
extern "C"{
#endif

/*=======================================================================
                                        INCLUDE FILES
=======================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Verification Test Environment Include Files */
#include "mx6s_v4l2_capture_uyvy.h"
#include <asm/types.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include <malloc.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#ifdef	GET_CONTI_PHY_MEM_VIA_PXP_LIB
#include "pxp_lib.h"
#endif

sigset_t sigset;
int quitflag;

#define  RGB(v) ({    \
			int value = (v); \
			(value > 0) * value | (255 * (value > 255));\
		})

#define TEST_BUFFER_NUM 3
#define MAX_V4L2_DEVICE_NR     64

struct testbuffer
{
	unsigned char *start;
	size_t offset;
	unsigned int length;
};

struct testbuffer buffers[TEST_BUFFER_NUM];
#ifdef	GET_CONTI_PHY_MEM_VIA_PXP_LIB
struct pxp_mem_desc mem[TEST_BUFFER_NUM];
#endif
int g_out_width = 320;
int g_out_height = 240;
int g_cap_fmt = V4L2_PIX_FMT_UYVY;
int g_capture_mode = 0;
int g_timeout = 10;
int g_camera_framerate = 15;	/* 30 fps */
int g_loop = 0;
int g_mem_type = V4L2_MEMORY_MMAP;
int g_frame_size;
char g_v4l_device[100] = "/dev/video1";
char g_saved_filename[100] = "1.yuv";
int  g_saved_to_file = 0;

struct fb_var_screeninfo var;
struct v4l2_buffer buf;

int frame_num = 0, fb0_size;
unsigned char *fb0;
struct timeval tv1, tv2;
int j = 0;
int out_w = 0, out_h = 0;
int bufoffset;
FILE * fd_y_file = 0;
unsigned char *cscbuf = NULL;
int gfd_v4l;

int start_capturing(int fd_v4l)
{

    if (g_saved_to_file == 1) {
        if ((fd_y_file = fopen(g_saved_filename, "wb")) == NULL) {
            printf("Unable to create y frame recording file\n");
            return -1;
        }
    }

    out_w = g_out_width;
    out_h = g_out_height;

    var.xres_virtual = var.xres;
    var.yres_virtual = 3 * var.yres;

    if (out_w > var.xres || out_h > var.yres) {
        printf("The output width or height is exceeding the resolution"
            " of the screen.\n"
            "wxh: %dx%d, screen wxh: %dx%d\n", out_w, out_h,
            var.xres, var.yres);
    }

     /* Map the device to memory*/
    /* allocate buffer for csc */
    cscbuf = malloc(out_w * out_h * 2);
    if (cscbuf == NULL) {
        printf("Unable to allocate cssbuf bytes\n");
    }

    var.yoffset = var.yres;
	unsigned int i;
	struct v4l2_buffer buf;
	enum v4l2_buf_type type;
	struct v4l2_requestbuffers req;

	memset(&req, 0, sizeof (req));
	req.count = TEST_BUFFER_NUM;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = g_mem_type;

	if (ioctl(fd_v4l, VIDIOC_REQBUFS, &req) < 0) {
		printf("VIDIOC_REQBUFS failed\n");
		return -1;
	}

	if (g_mem_type == V4L2_MEMORY_MMAP) {
		printf("%s: g_mem_type is memory_mmap\n", __func__);
		for (i = 0; i < TEST_BUFFER_NUM; i++) {
			memset(&buf, 0, sizeof (buf));
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = g_mem_type;
			buf.index = i;

			if (ioctl(fd_v4l, VIDIOC_QUERYBUF, &buf) < 0) {
				printf("VIDIOC_QUERYBUF error\n");
				return -1;
			}

			buffers[i].length = buf.length;
			buffers[i].offset = (size_t) buf.m.offset;
			buffers[i].start = mmap(NULL, buffers[i].length,
				PROT_READ | PROT_WRITE, MAP_SHARED,
				fd_v4l, buffers[i].offset);
	//		memset(buffers[i].start, 0xFF, buffers[i].length);
			/* add by Codebreaker */
			if(buffers[i].start == MAP_FAILED)
				return -1;
			/* add end */
		}
	}

	for (i = 0; i < TEST_BUFFER_NUM; i++)
	{
		memset(&buf, 0, sizeof (buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = g_mem_type;
		buf.index = i;
		buf.length = buffers[i].length;
		if (g_mem_type == V4L2_MEMORY_USERPTR)
			buf.m.userptr = (unsigned long) buffers[i].start;
		else
			buf.m.offset = buffers[i].offset;

		if (ioctl (fd_v4l, VIDIOC_QBUF, &buf) < 0) {
			printf("VIDIOC_QBUF error\n");
			return -1;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl (fd_v4l, VIDIOC_STREAMON, &type) < 0) {
		printf("VIDIOC_STREAMON error\n");
		return -1;
	}

	return 0;
}

int stop_capturing(int fd_v4l)
{
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	return ioctl (fd_v4l, VIDIOC_STREAMOFF, &type);
}

static int open_video_device(void)
{
	struct v4l2_capability cap;
	int fd_v4l;

	if ((fd_v4l = open(g_v4l_device, O_RDWR, 0)) < 0) {
		printf("unable to open %s for capture device.\n", g_v4l_device);
	}
	if (ioctl(fd_v4l, VIDIOC_QUERYCAP, &cap) == 0) {
		if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
			printf("Found v4l2 capture device %s.\n", g_v4l_device);
			return fd_v4l;
		}
	} else
		close(fd_v4l);

	return fd_v4l;
}

static void print_pixelformat(char *prefix, int val)
{
	printf("%s: %c%c%c%c\n", prefix ? prefix : "pixelformat",
					val & 0xff,
					(val >> 8) & 0xff,
					(val >> 16) & 0xff,
					(val >> 24) & 0xff);
}

void vl42_device_cap_list(void)
{
	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_frmivalenum frmival;
	struct v4l2_frmsizeenum frmsize;
	int fd_v4l = 0;
	char v4l_name[20];
	int i;

	for (i = 0; i < 5; i++) {
		snprintf(v4l_name, sizeof(v4l_name), "/dev/video%d", i);

		if ((fd_v4l = open(v4l_name, O_RDWR, 0)) < 0) {
			printf("\nunable to open %s for capture device.\n", v4l_name);
		} else
			printf("\nopen video device %s \n", v4l_name);

		if (ioctl(fd_v4l, VIDIOC_QUERYCAP, &cap) == 0) {
			if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
				printf("Found v4l2 capture device %s\n", v4l_name);
				fmtdesc.index = 0;
				fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				while (ioctl(fd_v4l, VIDIOC_ENUM_FMT, &fmtdesc) >= 0) {
					print_pixelformat("pixelformat (output by camera)",
							fmtdesc.pixelformat);
					frmsize.pixel_format = fmtdesc.pixelformat;
					frmsize.index = 0;
					while (ioctl(fd_v4l, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) {
				        frmival.index = 0;
						frmival.pixel_format = fmtdesc.pixelformat;
						frmival.width = frmsize.discrete.width;
						frmival.height = frmsize.discrete.height;
						while (ioctl(fd_v4l, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) >= 0) {
							printf("CaptureMode=%d, Width=%d, Height=%d %.3f fps\n",
									frmsize.index, frmival.width, frmival.height,
									1.0 * frmival.discrete.denominator / frmival.discrete.numerator);
							frmival.index++;
						}
						frmsize.index++;
					}
					fmtdesc.index++;
				}
			} else
				printf("Video device %s not support v4l2 capture\n", v4l_name);
		}
		close(fd_v4l);
	}
}

int v4l_capture_setup(void)
{
	struct v4l2_format fmt;
	struct v4l2_streamparm parm;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_frmsizeenum frmsize;
	int fd_v4l = 0;

	if ((fd_v4l = open_video_device()) < 0)
	{
		printf("Unable to open v4l2 capture device.\n");
		return -1;
	}

	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parm.parm.capture.capturemode = g_capture_mode;
	parm.parm.capture.timeperframe.denominator = g_camera_framerate;
	parm.parm.capture.timeperframe.numerator = 1;
	if (ioctl(fd_v4l, VIDIOC_S_PARM, &parm) < 0)
	{
		printf("VIDIOC_S_PARM failed\n");
		return -1;
	}

	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (ioctl(fd_v4l, VIDIOC_ENUM_FMT, &fmtdesc) < 0) {
		printf("VIDIOC ENUM FMT failed \n");
		close(fd_v4l);
		return -1;
	}
	print_pixelformat("pixelformat (output by camera)",
			fmtdesc.pixelformat);
	g_cap_fmt = fmtdesc.pixelformat;

	frmsize.pixel_format = fmtdesc.pixelformat;
	frmsize.index = g_capture_mode;
	if (ioctl(fd_v4l, VIDIOC_ENUM_FRAMESIZES, &frmsize) < 0) {
		printf("get capture mode %d framesize failed\n", g_capture_mode);
		return -1;
	}

	g_out_width = frmsize.discrete.width;
	g_out_height = frmsize.discrete.height;

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = g_cap_fmt;
	fmt.fmt.pix.width = g_out_width;
	fmt.fmt.pix.height = g_out_height;
	if (ioctl(fd_v4l, VIDIOC_S_FMT, &fmt) < 0)
	{
		printf("set format failed\n");
		return -1;
	}

	if (ioctl(fd_v4l, VIDIOC_G_FMT, &fmt) < 0)
	{
		printf("get format failed\n");
		return -1;
	}

	memset(&parm, 0, sizeof(parm));
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd_v4l, VIDIOC_G_PARM, &parm) < 0)
	{
		printf("VIDIOC_G_PARM failed\n");
		parm.parm.capture.timeperframe.denominator = g_camera_framerate;
	}

	printf("\t WxH@fps = %dx%d@%d", fmt.fmt.pix.width,
			fmt.fmt.pix.height, parm.parm.capture.timeperframe.denominator);
	printf("\t Image size = %d\n", fmt.fmt.pix.sizeimage);

	g_frame_size = fmt.fmt.pix.sizeimage;

    if (g_mem_type == V4L2_MEMORY_USERPTR)
        if (memalloc(g_frame_size, TEST_BUFFER_NUM) < 0) {
            close(fd_v4l);
        }
    gfd_v4l = fd_v4l;
	return fd_v4l;
}

void uyvytorgb565(unsigned char *uyvy, unsigned char *dst )
{
	int r0, g0, b0;
	int r1, g1, b1;
	int y0, y1, u, v;
	char *src;

	src = (char *)uyvy;
	u = *(src+0);
	y0 = *(src+1);
	v = *(src+2);
	y1 = *(src+3); 

	u = u - 128;
	v = v - 128;
	r0 = RGB(y0 + v + (v >> 2) + (v >> 3) + (v >> 5));
	g0 = RGB(y0 - ((u >> 2) + (u >> 4) + (u >> 5)) - (v >> 1) + (v >> 3) + (v >> 4) + (v >> 5));
	b0 = RGB(y0 + u + (u >> 1) + (u >> 2) + (u >> 6));

	r1 = RGB(y1 + v + (v >> 2) + (v >> 3) + (v >> 5));
	g1 = RGB(y1 - ((u >> 2) + (u >> 4) + (u >> 5)) - (v >> 1) + (v >> 3) + (v >> 4) + (v >> 5));
	b1 = RGB(y1 + u + (u >> 1) + (u >> 2) + (u >> 6));

	*(dst+1) = (r0 & 0xf8) | (g0 >> 5);
	*(dst) = ((g0 & 0x1c) << 3) | (b0 >> 3);

	*(dst+3) = (r1 & 0xf8) | (g1 >> 5);
	*(dst+2) = ((g1 & 0x1c) << 3) | (b1 >> 3);
}

void yuv32torgb565(unsigned char *yuv, unsigned char *dst )
{
	int r, g, b;
	int y, u, v;
	char *src;

	src = (char *)yuv;
	y = *(src+2);
	u = *(src+1);
	v = *(src+0);

	u = u - 128;
	v = v - 128;
	r = RGB(y + v + (v >> 2) + (v >> 3) + (v >> 5));
	g = RGB(y - ((u >> 2) + (u >> 4) + (u >> 5)) - (v >> 1) + (v >> 3) + (v >> 4) + (v >> 5));
	b = RGB(y + u + (u >> 1) + (u >> 2) + (u >> 6));

	*(dst+1) = (r & 0xf8) | (g >> 5);
	*(dst) = ((g & 0x1c) << 3) | (b >> 3);
}

void software_csc(unsigned char *inbuf, unsigned char *outbuf, int xres, int yres)
{
	unsigned char *yuv;
	unsigned char *rgb;
	int x;

	if (g_cap_fmt == V4L2_PIX_FMT_YUV32) {
		for (x = 0; x < xres*yres; x++) {
			yuv = inbuf + x*4;
			rgb = outbuf + x*2;
			yuv32torgb565(yuv, rgb);
		}
	} else if (g_cap_fmt == V4L2_PIX_FMT_UYVY) {
		for (x = 0; x < xres*yres/2; x++) {
			yuv = inbuf + x*4;
			rgb = outbuf + x*4;
			uyvytorgb565(yuv, rgb);
		}
	} else
		printf("Unsupport format in %s\n", __func__);
}

void software_csc_lmc(unsigned char *inbuf, unsigned char *outbuf, int xres, int yres)
{
    unsigned char *yuv;
    unsigned char *rgb;
    int x;
    int y;

    if (g_cap_fmt == V4L2_PIX_FMT_YUV32) {
        for (x = 0; x < xres*yres; x++) {
            yuv = inbuf + x*4;
            rgb = outbuf + x*2;
            yuv32torgb565(yuv, rgb);
        }
    } else if (g_cap_fmt == V4L2_PIX_FMT_UYVY) {
//        for (x = 0; x < xres*yres/2; x++) {
//            yuv = inbuf + x*4;
//            rgb = outbuf + x*4;
//            uyvytorgb565(yuv, rgb);
//        }
        for (y=0; y<yres; y++){
            for (x=0; x<xres/2; x++){
                yuv = inbuf + y*31104+x*24;
                rgb = outbuf + y*xres*2+x*4;
                uyvytorgb565(yuv, rgb);
            }
        }
//        for (y=0; y<yres; x++){
//            for (x=0; x<xres/2; x++){
//                yuv = inbuf ;//+ y*2560+x*4;
//                rgb = outbuf + y*480+x*4;
//                uyvytorgb565(yuv, rgb);
//            }
//        }
    } else
        printf("Unsupport format in %s\n", __func__);
}

int process_cmdline()
{
    g_capture_mode = 6;
    strcpy(g_v4l_device, "/dev/video0");
    g_saved_to_file = 0;
    g_timeout = 100;
}

static int signal_thread(void *arg)
{
	int sig;

	pthread_sigmask(SIG_BLOCK, &sigset, NULL);

	while (1) {
		sigwait(&sigset, &sig);
		if (sig == SIGINT) {
			printf("Ctrl-C received. Exiting.\n");
		} else {
			printf("Unknown signal. Still exiting\n");
		}
		quitflag = 1;
		break;
	}
	return 0;
}

#ifdef	GET_CONTI_PHY_MEM_VIA_PXP_LIB
void memfree(int buf_size, int buf_cnt)
{
	int i;
	unsigned int page_size;

	page_size = getpagesize();
	buf_size = (buf_size + page_size - 1) & ~(page_size - 1);

	for (i = 0; i < buf_cnt; i++) {
		if (buffers[i].start) {
			pxp_put_mem(&mem[i]);
			buffers[i].start = NULL;
		}
	}
	pxp_uninit();
}

int memalloc(int buf_size, int buf_cnt)
{
	int i, ret;
        unsigned int page_size;

	ret = pxp_init();
	if (ret < 0) {
		printf("pxp init err\n");
		return -1;
	}

	for (i = 0; i < buf_cnt; i++) {
		page_size = getpagesize();
		buf_size = (buf_size + page_size - 1) & ~(page_size - 1);
		buffers[i].length = mem[i].size = buf_size;
		ret = pxp_get_mem(&mem[i]);
		if (ret < 0) {
			printf("Get PHY memory fail\n");
			ret = -1;
			goto err;
		}
		buffers[i].offset = mem[i].phys_addr;
		buffers[i].start = (unsigned char *)mem[i].virt_uaddr;
		if (!buffers[i].start) {
			printf("mmap fail\n");
			ret = -1;
			goto err;
		}
		printf("%s, buf_size=0x%x\n", __func__, buf_size);
		printf("USRP: alloc bufs va=0x%x, pa=0x%x, size %d\n",
				buffers[i].start, buffers[i].offset, buf_size);
	}

	return ret;
err:
	memfree(buf_size, buf_cnt);
	return ret;
}
#else
void memfree(int buf_size, int buf_cnt) {}
int memalloc(int buf_size, int buf_cnt) { return 0; }
#endif

//int main(int argc, char **argv)
//{
//    int fd_v4l;
//    quitflag = 0;

////	pthread_t sigtid;
////	sigemptyset(&sigset);
////	sigaddset(&sigset, SIGINT);
////	pthread_sigmask(SIG_BLOCK, &sigset, NULL);
////	pthread_create(&sigtid, NULL, (void *)&signal_thread, NULL);

//    /* use input parm  */
//    if (process_cmdline() < 0) {
//        return -1;
//    }

//    fd_v4l = v4l_capture_setup();

//    if (fd_v4l < 0)
//        return -1;

//    v4l_capture_test(fd_v4l);

//    if (g_mem_type == V4L2_MEMORY_USERPTR)
//        memfree(g_frame_size, TEST_BUFFER_NUM);

//    return 0;
//}

int getRGV16Imag(unsigned char *img)
{
    static bool initStatus = false;
    if (!initStatus){
        system("/test_app/camera_ko/camera_install.sh");
        process_cmdline();
        v4l_capture_setup();
        start_capturing(gfd_v4l);
        initStatus = true;
    }
    memset(&buf, 0, sizeof (buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = g_mem_type;

    if (ioctl (gfd_v4l, VIDIOC_DQBUF, &buf) < 0) {
        printf("VIDIOC_DQBUF failed.\n");
    }

    software_csc_lmc(buffers[buf.index].start, img, 240, 320);
   // software_csc(buffers[buf.index].start, img, 240, 320);
  //  memcpy(buffers[buf.index].start, img, 240*320*2);

    if (ioctl (gfd_v4l, VIDIOC_QBUF, &buf) < 0) {
        printf("VIDIOC_QBUF failed\n");
    }

//    memset(&buf, 0, sizeof (buf));
//    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    buf.memory = g_mem_type;

//    if (ioctl (gfd_v4l, VIDIOC_DQBUF, &buf) < 0) {
//        printf("VIDIOC_DQBUF failed.\n");
//    }

//    software_csc_lmc(buffers[buf.index].start, img, 240, 320);
//   // memcpy(img, cscbuf, out_w*out_h*2);

//    if (ioctl (gfd_v4l, VIDIOC_QBUF, &buf) < 0) {
//        printf("VIDIOC_QBUF failed\n");
//    }
}

int imgCampture(unsigned char *img)
{
    enum v4l2_buf_type type;
    memset(&type,0,sizeof(type));
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl (gfd_v4l, VIDIOC_STREAMOFF, &type) < 0)
    {
        printf("TK--------VIDIOC_STREAMOFF >>>>>error 2　\r\n");
    }
    ///(10)解除内存映射
    unsigned int i = 0;
    for(i=0; i < TEST_BUFFER_NUM; ++i){
        if(-1 == munmap(buffers[i].start, buffers[i].length)){
            printf("munmap error! \n");
            exit(-1);
        }
    }

    close(gfd_v4l);

    struct v4l2_format fmt;
    struct v4l2_streamparm parm;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_frmsizeenum frmsize;

    if ((gfd_v4l = open_video_device()) < 0)
    {
        printf("Unable to open v4l2 capture device.\n");
        return -1;
    }

    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.capturemode = 6;
    parm.parm.capture.timeperframe.denominator = 15;
    parm.parm.capture.timeperframe.numerator = 1;
    if (ioctl(gfd_v4l, VIDIOC_S_PARM, &parm) < 0)
    {
        printf("VIDIOC_S_PARM failed\n");
        return -1;
    }

    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(gfd_v4l, VIDIOC_ENUM_FMT, &fmtdesc) < 0) {
        printf("VIDIOC ENUM FMT failed \n");
        close(gfd_v4l);
        return -1;
    }
    print_pixelformat("pixelformat (output by camera)",
            fmtdesc.pixelformat);

    frmsize.pixel_format = fmtdesc.pixelformat;
    frmsize.index = 6;
    if (ioctl(gfd_v4l, VIDIOC_ENUM_FRAMESIZES, &frmsize) < 0) {
        printf("get capture mode %d framesize failed\n", g_capture_mode);
        return -1;
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = fmtdesc.pixelformat;
    fmt.fmt.pix.width = frmsize.discrete.width;
    fmt.fmt.pix.height = frmsize.discrete.height;
    printf("fmt.fmt.pix.width = %d, fmt.fmt.pix.height = %d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);

    if (ioctl(gfd_v4l, VIDIOC_S_FMT, &fmt) < 0)
    {
        printf("set format failed\n");
        return -1;
    }

    if (ioctl(gfd_v4l, VIDIOC_G_FMT, &fmt) < 0)
    {
        printf("get format failed\n");
        return -1;
    }

    memset(&parm, 0, sizeof(parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(gfd_v4l, VIDIOC_G_PARM, &parm) < 0)
    {
        printf("VIDIOC_G_PARM failed\n");
        parm.parm.capture.timeperframe.denominator = 30;
    }

    printf("\t WxH@fps = %dx%d@%d", fmt.fmt.pix.width,
            fmt.fmt.pix.height, parm.parm.capture.timeperframe.denominator);
    printf("\t Image size = %d\n", fmt.fmt.pix.sizeimage);

    g_frame_size = fmt.fmt.pix.sizeimage;

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof (req));
    req.count = 3;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(gfd_v4l, VIDIOC_REQBUFS, &req) < 0) {
          printf("VIDIOC_REQBUFS failed\n");
          return -1;
      }

    struct buffer{
      void *start;
      unsigned int length;
    }tbuffers[3];

    unsigned int n_buffers = 0;

    for(n_buffers = 0; n_buffers < req.count; ++n_buffers){
      struct v4l2_buffer tbuf;
      memset(&buf,0,sizeof(tbuf));
      tbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      tbuf.memory = V4L2_MEMORY_MMAP;
      tbuf.index = n_buffers;

      if(ioctl(gfd_v4l,VIDIOC_QUERYBUF,&tbuf) == -1){
        printf("TK---------_>>>>>>error\n");
        close(gfd_v4l);
        exit(-1);
      }
          //(6)映射内核空间到用户空间
      tbuffers[n_buffers].length = tbuf.length;
      tbuffers[n_buffers].start = mmap(NULL, tbuf.length,PROT_READ|PROT_WRITE,MAP_SHARED, gfd_v4l, tbuf.m.offset);

      if(MAP_FAILED == tbuffers[n_buffers].start){
        printf("TK--------__>>>>>error 2\n");
        close(gfd_v4l);
        exit(-1);
      }
          memset(tbuffers[n_buffers].start,0,tbuffers[n_buffers].length);

          if(ioctl(gfd_v4l,VIDIOC_QBUF,&tbuf) < 0 )
          {
              printf("TK--------VIDIOC_QBUF>>>>>error 2\r\n");
          }
    }
    ///(7)打开视频捕获
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if(ioctl(gfd_v4l,VIDIOC_STREAMON,&type) < 0)
      {
          printf("TK--------VIDIOC_STREAMON>>>>>error 2　\r\n");
      }

    ////
      ///(8)把数据放回缓存队列
    memset(&buf, 0, sizeof (buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl (gfd_v4l, VIDIOC_DQBUF, &buf) < 0) {
        printf("VIDIOC_DQBUF failed.\n");
    }

    software_csc(tbuffers[buf.index].start, img, 2592, 1944);

    if (ioctl (gfd_v4l, VIDIOC_QBUF, &buf) < 0) {
        printf("VIDIOC_QBUF failed\n");
    }

    memset(&type,0,sizeof(type));
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl (gfd_v4l, VIDIOC_STREAMOFF, &type) < 0)
    {
        printf("TK--------VIDIOC_STREAMOFF >>>>>error 2　\r\n");
    }
    ///(10)解除内存映射
    for(i=0; i < TEST_BUFFER_NUM; ++i){
        if(-1 == munmap(tbuffers[i].start, tbuffers[i].length)){
            printf("munmap error! \n");
            exit(-1);
        }
    }

    close(gfd_v4l);

    process_cmdline();
    v4l_capture_setup();
    start_capturing(gfd_v4l);

}

void getRGV16ImaCapture(unsigned char *img)
{
    memset(&buf, 0, sizeof (buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = g_mem_type;

    if (ioctl (gfd_v4l, VIDIOC_DQBUF, &buf) < 0) {
        printf("VIDIOC_DQBUF failed.\n");
    }

    software_csc(buffers[buf.index].start, img, 2592, 1944);
   // memcpy(img, cscbuf, out_w*out_h*2);

    if (ioctl (gfd_v4l, VIDIOC_QBUF, &buf) < 0) {
        printf("VIDIOC_QBUF failed\n");
    }
}


