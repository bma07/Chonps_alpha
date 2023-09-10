#ifndef	HG_CHONPS_GUI_WINDOW_H
#define HG_CHONPS_GUI_WINDOW_H

namespace Chonps
{
	class GuiWindow
	{
	public:
		void Draw();
		void SetPos(float inx, float iny) { x = inx; y = iny; }
		void SetDimensions(float inWidth, float inHeight) { width = inWidth; height = inHeight; }

		float x, y, width, height;

	private:

	};
}

#endif