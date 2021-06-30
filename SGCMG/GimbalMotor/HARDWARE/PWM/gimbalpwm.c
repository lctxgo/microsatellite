#include "gimbalpwm.h"

void gimbalPWMInit(u16 arr, u16 psc) {
	TIM3_Init(arr, psc);
}
