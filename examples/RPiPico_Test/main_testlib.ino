/** ----------------------------------------------------------------------------
Copyright 2020-2021 Tangibles That Teach, LLC, 4617 Chattahoochee Crossing SE,
Marietta, GA 30067, U.S.A. All rights reserved.

Tangibles That Teach, LLC has intellectual property rights relating to
technology embodied in this product. In particular, and without
limitation, these intellectual property rights may include one or more
of U.S. patents or pending patent applications in the U.S. and/or other countries.

This product is distributed under licenses restricting its use, copying
distribution, and decompilation. No part of this product may be
reproduced in any form by any means without prior written authorization
of Tangibles That Teach, LLC.

Contact Info:
Email: contact@tangiblesthatteach.com

@brief: Main program for the T-RECS running on the Raspberry Pi Pico board
@author: Joel Dunham <joel.dunham@tangiblesthatteach.com>
@date: 2021/04/07
------------------------------------------------------------------------------ **/

#include <t3Library.h>

T3Library::T3Library t3Library;

void setup() {
	t3Library.setup();
}

void loop() {
	t3Library.update();
}
