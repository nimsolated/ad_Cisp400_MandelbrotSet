#include "ComplexPlane.h"

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
	m_pixel_size = { pixelWidth, pixelHeight };
	m_aspectRatio = pixelHeight / pixelWidth;
	m_plane_center = { 0, 0 };
	m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio };
	m_zoomCount = 0;
	m_state = State::CALCULATING;
	
	VertexArray va(Points);
	va.resize(pixelWidth * pixelHeight);
	m_vArray = va;
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
	target.draw(m_vArray);
}

// TODO: defining the rest of the member functions

void ComplexPlane::updateRender()
{
	if (m_state == State::CALCULATING)
	{
		for (int i = 0; i < m_plane_size.y; i++)
		{
			for (int j = 0; j < m_plane_size.x; i++)
			{
				m_vArray[j + i * m_pixel_size.x].position = { (float)j, (float)i };
				Vector2f coords = mapPixelToCoords({ j, i });
				size_t iters = countIterations(coords);
				Uint8 r, g, b;
				iterationsToRGB(iters, r, g, b);
				m_vArray[j + i * m_pixel_size.x].color = { r, g, b };
			}
		}

		m_state = State::DISPLAYING;
	}
}

void ComplexPlane::zoomIn()
{
	m_zoomCount++;
	float x_size = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
	float y_size = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));
	m_plane_size = { x_size, y_size };
	m_state = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
	m_zoomCount--;
	float x_size = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
	float y_size = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));
	m_plane_size = { x_size, y_size };
	m_state = State::CALCULATING;
}

void ComplexPlane::setCenter(Vector2i mousePixel)
{
	m_plane_center = mapPixelToCoords(mousePixel);
	m_state = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
	m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text)
{
	string title = "Mandelbrot Set\n";
	string center = "Center: (" + to_string(m_plane_center.x) + ", " + to_string(m_plane_center.y) + ")\n";
	string cursor = "Cursor: (" + to_string(m_mouseLocation.x) + ", " + to_string(m_mouseLocation.y) + ")\n";
	string leftClick = "Left-click to Zoom in\n";
	string rightClick = "Right-click to Zoom out\n";

	text.setString(title + center + cursor + leftClick + rightClick);
}

size_t ComplexPlane::countIterations(Vector2f coord)
{
	complex<double> c(coord.x, coord.y);
	complex<double> z = c;
	size_t iters = 0;

	/*
	* * * * FOR MULTITHREADING LATER
	* 
	vector<thread> threads;
	const int numThreads = 4;
	*/

	while (abs(z) < 2.0 && iters < MAX_ITER)
	{
		z = z * z + c;
		/*
		* * * * FOR MULTITHREADING LATER
		* 
		for (int i = 0; i < numThreads; ++i)
		{
			threads.push_back(thread(incrementIters, iters));
		}
		for (auto& t : threads)
		{
			t.join();
		}
		*/
		iters++;
	}

	return iters;
}

/*
* * * * FOR MULTITHREADING LATER
* 
void incrementIters(size_t& iters)
{
	mutex mtx;
	lock_guard<mutex> lock(mtx);
	iters++;
}
*/

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
	if (count == MAX_ITER)
	{
		r = 0;
		g = 0;
		b = 0;
	}
	else if (count < MAX_ITER)
	{
		if (count > MAX_ITER * 0.5)
		{
			r = 100;
			g = 20;
			b = 20;
		}
		else if (count > MAX_ITER * 0.25)
		{
			r = 200;
			g = 40;
			b = 40;
		}
		else if (count > MAX_ITER * 0.1275)
		{
			r = 255;
			g = 127;
			b = 127;
		}
		else
		{
			r = 255;
			g = 255;
			b = 255;
		}
	}
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
	Vector2f coords(0.0, 0.0);

	coords.x = ((float)mousePixel.x / m_pixel_size.x) * m_plane_size.x - m_plane_size.x / 2;
	coords.y = ((float)mousePixel.y / m_pixel_size.y) * m_plane_size.y - m_plane_size.y / 2;

	return coords;
}