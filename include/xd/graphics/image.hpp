#ifndef H_XD_GRAPHICS_IMAGE
#define H_XD_GRAPHICS_IMAGE

#include <xd/graphics/detail/image.hpp>

#include <xd/config.hpp>
#include <xd/ref_counted.hpp>
#include <boost/noncopyable.hpp>
#include <boost/intrusive_ptr.hpp>
#include <string>
#include <memory>

#ifndef XD_STATIC
// disable warning about boost::noncopyable not being dll-exportable
// as well as the private members that can't be accessed by client
#pragma warning(disable: 4275 4251)
#endif

namespace xd
{
	class XD_API image : public xd::ref_counted, public boost::noncopyable
	{
	public:
		typedef boost::intrusive_ptr<image> pt;

		image(const std::string& filename);
		virtual ~image();

		void load(const std::string& filename);
		void save(const std::string& filename) const;

		int width() const { return m_width; }
		int height() const { return m_height; }

		std::string filename() const { return m_filename; }

		void *data();
		const void *data() const;

	private:
		detail::image::handle_ptr m_image;
		int m_width;
		int m_height;
		std::string m_filename;

		void init();
	};
}

#endif
