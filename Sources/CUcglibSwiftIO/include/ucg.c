//
//  ucg.c
//  
//
//  Created by Omer Shamai on 24/10/2022.
//

#include "ucg.h"

ucg_int_t ucg_GetWidth(ucg_t *ucg) {
    return ucg->dimension.w;
}

ucg_int_t ucg_GetHeight(ucg_t *ucg) {
    return ucg->dimension.h;
}
