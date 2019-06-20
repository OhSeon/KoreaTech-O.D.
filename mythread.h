#pragma once

#include "stdafx.h"
#include "SharedImageBuffer.h"
#include "MFCApplication1.h"

using std::string;

using namespace cv;
using namespace boost::interprocess;
using namespace boost::detail;

bool paused = false;
bool played = false;

void program_thread(std::stringstream & cmd)
{
	std::system(cmd.str().c_str());
}

void _camera(ti &o) {
	bool image_null = true;

		while (1) {
			if(played)
				image_null = o.cam.vc.read(o.cam.image);

			if (image_null==false)
				break;

			if (!o.od->index) {
				memcpy(o.od->SIBimage[0]._pMem, o.cam.image.data, o.od->dsz);
			}
			else {
				memcpy(o.od->SIBimage[1]._pMem, o.cam.image.data, o.od->dsz);
			}

			o.od->ready_flag = true;
			boost::detail::Sleep(40);
		}
}