/************************************************************************ 
 *
 * AROS Drawing
 *
 * Copyright 2011 Oliver Brunner - aros<at>oliver-brunner.de
 *
 * This file is part of Janus-UAE.
 *
 * Janus-UAE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Janus-UAE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Janus-UAE. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id: gfx.cpp 1132 2014-12-23 14:45:00Z kalamatee $
 *
 ************************************************************************/

 /* THIS IS OBSOLETE. USE arosgfx.cpp!! */


#include <proto/intuition.h>
#include <proto/graphics.h>

#include <libraries/cybergraphics.h>
#include <proto/cybergraphics.h>

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "audio.h"
#include "uae.h"
#include "memory.h"
#include "custom.h"
#include "events.h"
#include "newcpu.h"
#include "traps.h"
#include "xwin.h"
#include "keyboard.h"
#include "drawing.h"
#include "gfx.h"
#include "picasso96_aros.h"
#include "inputdevice.h"
#include "gui.h"
#include "serial.h"
#include "gfxfilter.h"
#include "sampler.h"
#include "sounddep/sound.h"
#ifdef RETROPLATFORM
#include "rp.h"
#endif

//struct Window *hAmigaWnd;
//struct RastPort  *TempRPort;

static uae_u8 scrlinebuf[4096 * 4]; /* this is too large, but let's rather play on the safe side here */

static BOOL doInit (void);

#if 0
int graphics_setup (void) {

	/* TODO(); */

#ifdef PICASSO96
	//InitPicasso96 ();
#endif

	/* j-uae calls
	init_pointer ();
	initpseudodevices ();
	 * here */
	return 1;
}
#endif

static void gfxmode_reset (void) {
	TODO();
}

/* debug helper */

void dump_description(vidbuf_description *vid) {
  bug("----------------------\n");
  bug("dump vidbuf_description %lx\n", vid);
/*
  bug("----------------------\n");
  bug("flush_line: %lx\n", vid->flush_line);
  bug("flush_block: %lx\n", vid->flush_block);
  bug("flush_screen: %lx\n", vid->flush_screen);
  bug("flush_clear_screen: %lx\n", vid->flush_clear_screen);
  bug("lockscr: %lx\n", vid->lockscr);
  bug("unlockscr: %lx\n", vid->unlockscr);
  bug("----------------------\n");
  bug("bufmem: %lx\n", vid->bufmem);
  bug("realbufmem: %lx\n", vid->realbufmem);
  bug("linemem: %lx\n", vid->linemem);
  bug("emergmem: %lx\n", vid->emergmem);
  bug("rowbytes: %d\n", vid->rowbytes);
  bug("pixbytes: %d\n", vid->pixbytes);
  bug("width: %d\n", vid->width);
  bug("height: %d\n", vid->height);
  bug("maxblocklines: %d\n", vid->maxblocklines);
  bug("gfx_resolution_reserved: %d\n", vid->gfx_resolution_reserved);
  bug("gfx_vresolution_reserved: %d\n", vid->gfx_vresolution_reserved);
*/
  bug("----------------------\n");
}

#if 0
static int create_windows_2 (void) {
	/* window borders */
	int cy_top_border    = 10; /* TODO: get correct values!? */
	int cy_bottom_border = 10;
	int cx_left_border   = 10;
	int cx_right_border  = 10;

	if(hAmigaWnd) {
		/* we already have a window.. TODO */
		TODO();
		return 1;
	}

	currentmode->amiga_width=currentmode->current_width;
	currentmode->amiga_height=currentmode->current_height;

	bug("opening window with %dx%d\n", currentmode->current_width, currentmode->current_height);

	hAmigaWnd= OpenWindowTags (NULL,
			  WA_Title,        (ULONG)PACKAGE_NAME,
			  WA_AutoAdjust,   TRUE,
			  WA_InnerWidth,   currentmode->amiga_width,
			  WA_InnerHeight,  currentmode->amiga_height,
			  //WA_PubScreen,    (ULONG)S,
			  //WA_Zoom,         (ULONG)ZoomArray,
			  WA_IDCMP,        IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY
					 | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW
					 | IDCMP_MOUSEMOVE    | IDCMP_DELTAMOVE
					 | IDCMP_CLOSEWINDOW  | IDCMP_REFRESHWINDOW
					 | IDCMP_NEWSIZE      | IDCMP_INTUITICKS,
			  WA_Flags,	   WFLG_DRAGBAR       | WFLG_DEPTHGADGET
					 | WFLG_REPORTMOUSE   | WFLG_RMBTRAP
					 | WFLG_ACTIVATE      | WFLG_CLOSEGADGET
					 | WFLG_SMART_REFRESH,
			  TAG_DONE);

	bug("hAmigaWnd: %lx\n", hAmigaWnd);

	if(!hAmigaWnd) {
		return 0;
	}

	return 1;
}

static void close_windows (void) {

	bug("entered\n");

	if(hAmigaWnd) {
		CloseWindow(hAmigaWnd);
		hAmigaWnd=NULL;
	}

}

static int create_windows (void) {
	if (!create_windows_2 ()) {
		return 0;
	}

	/* call set_ddraw here ..*/

	return 1;
}


static BOOL doInit (void) {
	BOOL ret;

	/* LOT's of stuff to do here! */

	bug("LOT's of stuff to do here..\n");

//OOPS:
	if(!create_windows ()) {
		bug("unable to open windows!\n");
		goto OOPS;
	}

	for(;;) {
		bug("for(;;)..\n");
		if(screen_is_picasso) {
			break;
		}
		else {

       /* quote from xwin.h:
        *
        * The graphics code has a choice whether it wants to use a large buffer
        * for the whole display, or only a small buffer for a single line.
        * If you use a large buffer:
        *   - set bufmem to point at it
        *   - set linemem to 0
        *   - if memcpy within bufmem would be very slow, i.e. because bufmem is
        *     in graphics card memory, also set emergmem to point to a buffer
        *     that is large enough to hold a single line.
        *   - implement flush_line to be a no-op.
        * If you use a single line buffer:
        *   - set bufmem and emergmem to 0
        *   - set linemem to point at your buffer
        *   - implement flush_line to copy a single line to the screen
        */
			currentmode->amiga_width = currentmode->current_width;
			currentmode->amiga_height = currentmode->current_height;

			bug("currentmode->amiga_width: %d\n", currentmode->amiga_width);
			bug("currentmode->amiga_height: %d\n", currentmode->amiga_height);

      bug("hAmigaWnd: %lx\n", hAmigaWnd);
      if(!hAmigaWnd) {
        write_log("hAmigaWnd is NULL !?\n");
        bug("hAmigaWnd is NULL !?\n");
        return 0;
      }

			gfxvidinfo.width = (currentmode->amiga_width + 7) & ~7;
			gfxvidinfo.height = currentmode->amiga_height;

      bug("gfxvidinfo.width : %4d\n", gfxvidinfo.width);
      bug("gfxvidinfo.height: %4d\n", gfxvidinfo.height);

			gfxvidinfo.maxblocklines = 0; // flush_screen actually does everything

      gfxvidinfo.pixbytes = GetCyberMapAttr (hAmigaWnd->RPort->BitMap, CYBRMATTR_BPPIX);
      bug("gfxvidinfo.pixbytes: %d\n", gfxvidinfo.pixbytes);
			gfxvidinfo.rowbytes = GetCyberMapAttr (hAmigaWnd->RPort->BitMap, CYBRMATTR_XMOD);
      bug("gfxvidinfo.rowbytes: %d\n", gfxvidinfo.rowbytes);

			gfxvidinfo.bufmem = (uae_u8 *) AllocVec (gfxvidinfo.rowbytes * gfxvidinfo.height, MEMF_ANY);
      /* TODO: We need to free it again!!! */
      bug("gfxvidinfo.bufmem: %lx\n", gfxvidinfo.bufmem);
      gfxvidinfo.realbufmem = gfxvidinfo.bufmem; // necessary !? TODO!

      if(!gfxvidinfo.bufmem) {
        write_log("could not alloc gfxvidinfo.bufmem!\n");
        bug("could not alloc gfxvidinfo.bufmem!\n");
        return 0;
      }

			gfxvidinfo.linemem = 0;

#if 0
			oldpixbuf = xmalloc (uae_u8, size);
      /* we use option 2! */
      /* but D3D uses option one, so maybe we should, tpp ..?*/
			gfxvidinfo.realbufmem = 0;
			gfxvidinfo.bufmem     = 0;
			gfxvidinfo.emergmem   = 0;

			gfxvidinfo.linemem    = xmalloc(uae_u8, gfxvidinfo.rowbytes);

#endif
			//gfxvidinfo.rowbytes = currentmode->pitch;
			break;
		}
	} /* for */

/* disabled now:
	gfxvidinfo.emergmem = scrlinebuf; // memcpy from system-memory to video-memory
 */

  dump_description(&gfxvidinfo);

	return 1;

OOPS:
	close_windows();
	return ret;
}

static void updatemodes(void) {
	WORD flags;

	bug("entered\n");

	/* HACK: */
	if(!currentmode->current_width) {
		bug("hardsetting currentmode->current_width to currprefs.gfx_size.width..\n");
		currentmode->current_width =currprefs.gfx_size.width;
		currentmode->current_height=currprefs.gfx_size.height;
	}
#if 0
	if (flags & DM_W_FULLSCREEN) {
		RECT rc = getdisplay (&currprefs)->rect;
		currentmode->native_width = rc.right - rc.left;
		currentmode->native_height = rc.bottom - rc.top;
		currentmode->current_width = currentmode->native_width;
		currentmode->current_height = currentmode->native_height;
	} else {
#endif
		currentmode->native_width = currentmode->current_width;
		currentmode->native_height = currentmode->current_height;
		bug("currentmode->current_width: %d\n", currentmode->current_width);
		bug("currentmode->native_width: %d\n", currentmode->native_width);

    /* TODO: is this a good idea: */
    bug("setting currentmode->fullfill to 1 .. ??\n");
    currentmode->fullfill = 1;
#if 0
	}
#endif
}


static void update_gfxparams (void) {

	bug("entered\n");

	//updatewinfsmode (&currprefs);
#ifdef PICASSO96
	currentmode->vsync = 0;
	if (screen_is_picasso) {
		currentmode->current_width = picasso96_state.Width;
		currentmode->current_height = picasso96_state.Height;
		currentmode->frequency = abs (currprefs.gfx_refreshrate > default_freq ? currprefs.gfx_refreshrate : default_freq);
		if (currprefs.gfx_pvsync)
			currentmode->vsync = 1;
	} else {
#endif
		currentmode->current_width = currprefs.gfx_size.width;
		bug("currprefs.gfx_size.width: %d\n", currprefs.gfx_size.width);
		bug("currprefs.gfx_size.width_windowed: %d\n", currprefs.gfx_size_win.width);
		currentmode->current_height = currprefs.gfx_size.height;
		currentmode->frequency = abs (currprefs.gfx_refreshrate);
		if (currprefs.gfx_avsync)
			currentmode->vsync = 1;
#ifdef PICASSO96
	}
#endif
	currentmode->current_depth = currprefs.color_mode < 5 ? 16 : 32;
	if (screen_is_picasso && currprefs.win32_rtgmatchdepth && isfullscreen () > 0) {
		int pbits = picasso96_state.BytesPerPixel * 8;
		if (pbits <= 8) {
			if (currentmode->current_depth == 32)
				pbits = 32;
			else
				pbits = 16;
		}
		if (pbits == 24)
			pbits = 32;
		currentmode->current_depth = pbits;
	}
	currentmode->amiga_width = currentmode->current_width;
	currentmode->amiga_height = currentmode->current_height;

	scalepicasso = 0;
	if (screen_is_picasso) {
		if (isfullscreen () < 0) {
			if ((currprefs.win32_rtgscaleifsmall || currprefs.win32_rtgallowscaling) && (picasso96_state.Width != currentmode->native_width || picasso96_state.Height != currentmode->native_height))
				scalepicasso = 1;
			if (!scalepicasso && currprefs.win32_rtgscaleaspectratio)
				scalepicasso = -1;
		} else if (isfullscreen () > 0) {
			if (!currprefs.win32_rtgmatchdepth) { // can't scale to different color depth
				if (currentmode->native_width > picasso96_state.Width && currentmode->native_height > picasso96_state.Height) {
					if (currprefs.win32_rtgscaleifsmall)
						scalepicasso = 1;
				}
				if (!scalepicasso && currprefs.win32_rtgscaleaspectratio)
					scalepicasso = -1;
			}
		} else if (isfullscreen () == 0) {
			if ((currprefs.gfx_size.width != picasso96_state.Width || currprefs.gfx_size.height != picasso96_state.Height) && currprefs.win32_rtgallowscaling)
				scalepicasso = 1;
			if ((currprefs.gfx_size.width > picasso96_state.Width || currprefs.gfx_size.height > picasso96_state.Height) && currprefs.win32_rtgscaleifsmall)
				scalepicasso = 1;
			if (!scalepicasso && currprefs.win32_rtgscaleaspectratio)
				scalepicasso = -1;
		}

		if (scalepicasso > 0 && (currprefs.gfx_size.width != picasso96_state.Width || currprefs.gfx_size.height != picasso96_state.Height)) {
			currentmode->current_width = currprefs.gfx_size.width;
			currentmode->current_height = currprefs.gfx_size.height;
		}
	}

}

static int init_round;

static int open_windows (int full) {

	int ret;

  bug("full: %d\n", full);

	inputdevice_unacquire ();
	reset_sound ();

	init_round = 0; /* ? */
	ret = -2;
	do {
		bug("loop while ret<0: ret: %d, init_round: %d\n", ret, init_round);
		if(ret < -1) {
			updatemodes ();
			update_gfxparams ();
		}
		ret = doInit ();
		init_round++;
	}
	while (ret < 0);

	if(!ret) {
		bug("open_windows failed: ret==0\n");
		return ret;
	}

	inputdevice_acquire (TRUE);

  bug("result: %d SUCCESS\n", ret);

	return ret;
}

#if 0
void flush_clear_screen_gfxlib(vidbuf_description *vidbuf) {
	bug("entered\n");

}

static void flush_line_planar_nodither (struct vidbuf_description *gfxinfo, int line_no) {

	bug("entered\n");

}


static void flush_block_planar_nodither (struct vidbuf_description *gfxinfo, int first_line, int last_line)
{
	int line_no;
	bug("entered\n");
}
#endif
#endif

/* graphics_init does not very much, just gfxmode_reset and open_windows */
#if 0
int graphics_init(void) {

	bug("entered\n");

	/* TODO: free it again */
	TempRPort = (struct RastPort *) AllocVec (sizeof (struct RastPort), MEMF_ANY | MEMF_PUBLIC);
	if (!TempRPort) {
		write_log ("Unable to allocate RastPort.\n");
		return 0;
	}

	gfxmode_reset ();

	/* in WinUAE a lot of stuff is configured by the GUI (read from PREFS->written to workin prefs etc.
	 * we do this here for the moment:
	 */
	currprefs.gfx_size.width=currprefs.gfx_size_win.width;
	currprefs.gfx_size.height=currprefs.gfx_size_win.height;

	return open_windows (1);
}
#endif

#if 0
int isscreen (void)
{
	return hAmigaWnd ? 1 : 0;
}
#endif

/* not sure, what this does..
 * winuae uses it to D3D_locktexture/DirectDraw_SurfaceLock stuff.
 * I doubt, we need this on AROS !?
 */
int lockscr (int fullupdate) {
    bug("[JUAE:Gfx] %s(%d)\n", __PRETTY_FUNCTION__, fullupdate);

  TODO();
  return 1;
#if 0
	if(!isscreen()) {
		bug("no screen\n");
		return 0;
	}
	return 1;
#endif
}

void enumeratedisplays (int multi) {
    bug("[JUAE:Gfx] %s()\n", __PRETTY_FUNCTION__);

  if (multi) {
    /*
    int cnt = 1;
    DirectDraw_EnumDisplays (displaysCallback);
    EnumDisplayMonitors (NULL, NULL, monitorEnumProc, (LPARAM)&cnt);
    */
    write_log (_T("ERROR: Multimonitor does not work!\n"));
  }
  write_log (_T("Multimonitor detection disabled\n"));
  Displays[0].primary = 1;
  Displays[0].adaptername = my_strdup(_T("Display"));
  Displays[0].monitorname = my_strdup(_T("Display"));

  //Displays[0].disabled = 0;
}

#if 0
static void flushit (int line_no) {

	int     xs      = 0;
  int     last    = 0;
	int     len     = gfxvidinfo.width;
	int     yoffset = line_no * gfxvidinfo.rowbytes;
	uae_u8 *src;
	uae_u8 *dst;
	//uae_u8 *newp = gfxvidinfo.rowbytes.bufmem + yoffset;
	//uae_u8 *oldp = oldpixbuf + yoffset;

	bug("entered\n");

#if 0
	if(uae_no_display_update) {
		return;
	}

	/* Find first pixel changed on this line */
	while (*newp++ == *oldp++) {
if (!--len)
		return; /* line not changed - so don't draw it */
	}
	src   = --newp;
	dst   = --oldp;
	newp += len;
	oldp += len;

	/* Find last pixel changed on this line */
	while (*--newp == *--oldp)
;

	len = 1 + (oldp - dst);
	xs  = src - (uae_u8 *)(gfxinfo->bufmem + yoffset);

#endif

	/* Copy changed pixels to delta buffer */
	//CopyMem (src, dst, len);



	/* Blit changed pixels to the display */
	bug("WritePixelLine8 ..\n");
  TODO();
#if 0
	WritePixelLine8 (hAmigaWnd->RPort, 0, line_no, len, gfxvidinfo.bufmem, TempRPort);
#if 0
	WritePixelLine8 (hAmigaWnd->RPort, 0, line_no, len, gfxvidinfo.linemem, TempRPort);
  bug("gfxvidinfo.linemem:\n");
  for(xs=0; xs<len;xs++) {
    bug(" 0x%02x", *(gfxvidinfo.linemem + line_no*gfxvidinfo.width + xs));
  }
  bug("\n");
#endif
  bug("gfxvidinfo.bufmem:\n");
  for(xs=0; xs<len;xs++) {
    if(*(gfxvidinfo.bufmem + line_no*gfxvidinfo.rowbytes + xs) == 0) {
        last++;
    }
    else {
      if(last>0) {
        bug("\n%d zero values skipped\n", last);
        last=0;
      }
      bug(" 0x%02x", *(gfxvidinfo.bufmem + line_no*gfxvidinfo.rowbytes + xs));
    }
  }
  if(last>0) {
    bug("\n%d zero values skipped\n", last);
  }
  else {
    bug("\n");
  }
#endif



}

void flush_line(int lineno) {

/* TODO */
//	if(lineno >100 && lineno < 140) {
		flushit (lineno);
//	}
}
#endif
