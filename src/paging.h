/* 
 * This file is part of TermOS.
 *
 * TermOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TermOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TermOS.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _PAGING_H
#define _PAGING_H

#include "common.h"

typedef struct
{
    u32 present  : 1;  /* Page present in memory */
    u32 rw       : 1;  /* Read-only if clear, readwrite if set */
    u32 user     : 1;  /* Supervisor level only if clear */
    u32 accessed : 1;  /* Has the page been accessed since last refresh? */
    u32 dirty    : 1;  /* Has the page been written to since last refresh? */
    u32 unused   : 7;  /* Amalgamation of unused and reserved bits */
    u32 frame    : 20; /* Frame address (shifted right 12 bits) */
} page_t;

typedef struct
{
    page_t pages[1024];
} page_table_t;

typedef struct
{
    page_table_t* tables[1024];
    u32 tablesPhysical[1024];
    u32 physicalAddr;
} page_directory_t;

void init_paging();
void switch_page_directory(page_directory_t*);
page_t *get_page(u32, int, page_directory_t*);
void page_fault(registers_t*);

#endif
