/******************************************************************************
  Copyright 2014 Tom Wong-Cornall

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 ******************************************************************************/
#ifndef REPORTS_H
#define REPORTS_H

typedef struct {
	uint8_t modifiers;
	uint8_t codeBmp[21];
} __attribute__((packed)) NKROReport;

typedef struct {
	uint8_t codeBmp;
} __attribute__((packed)) ExtrakeySystemReport;

typedef struct {
	uint8_t codeBmp[3];
} __attribute__((packed)) ExtrakeyConsumerReport;

#endif
