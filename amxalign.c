/*  Pawn Abstract Machine (for the Pawn language)
 *
 *  Copyright (c) ITB CompuPhase, 1997-2005
 *
 *  This software is provided "as-is", without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising from
 *  the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1.  The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software in
 *      a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *  2.  Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *  3.  This notice may not be removed or altered from any source distribution.
 *
 *  Version: $Id: amx.c 3365 2005-07-23 15:57:48Z thiadmer $
 */

#include <assert.h>

#include "amx/amx.h"
#include "amx/osdefs.h"

#if !defined NDEBUG
  static int check_endian(void)
  {
    uint16_t val=0x00ff;
    unsigned char *ptr=(unsigned char *)&val;
    /* "ptr" points to the starting address of "val". If that address
     * holds the byte "0xff", the computer stored the low byte of "val"
     * at the lower address, and so the memory lay out is Little Endian.
     */
    assert(*ptr==0xff || *ptr==0x00);
    #if BYTE_ORDER==BIG_ENDIAN
      return *ptr==0x00;  /* return "true" if big endian */
    #else
      return *ptr==0xff;  /* return "true" if little endian */
    #endif
  }
#endif

#if BYTE_ORDER==BIG_ENDIAN 
  static void swap16(uint16_t *v)
  {
    unsigned char *s = (unsigned char *)v;
    unsigned char t;

    assert(sizeof(*v)==2);
    /* swap two bytes */
    t=s[0];
    s[0]=s[1];
    s[1]=t;
  }
#endif

#if BYTE_ORDER==BIG_ENDIAN 
  static void swap32(uint32_t *v)
  {
    unsigned char *s = (unsigned char *)v;
    unsigned char t;

    assert(sizeof(*v)==4);
    /* swap outer two bytes */
    t=s[0];
    s[0]=s[3];
    s[3]=t;
    /* swap inner two bytes */
    t=s[1];
    s[1]=s[2];
    s[2]=t;
  }
#endif

uint16_t * AMXAPI amx_Align16_(uint16_t *v)
{
  assert(sizeof(*v)==2);
  assert(check_endian());
  #if BYTE_ORDER==BIG_ENDIAN
    swap16(v);
  #endif
  return v;
}

uint32_t * AMXAPI amx_Align32_(uint32_t *v)
{
  assert(sizeof(*v)==4);
  assert(check_endian());
  #if BYTE_ORDER==BIG_ENDIAN
    swap32(v);
  #endif
  return v;
}
