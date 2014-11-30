#ifndef _TLV5614_H_
#define _TLV5614_H_

#include "MyF5529.h"

#define TLV5614_REF			3.64

#define TLV5614_CHA			0
#define TLV5614_CHB			1
#define TLV5614_CHC			2
#define TLV5614_CHD			3

extern void TLV5614_Init();
extern void TLV5614_DAConverter(unsigned char address,float voltage);

#endif /* TLV5614_H_ */
