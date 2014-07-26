#ifndef H_XD_GRAPHICS_TEXTURE
#define H_XD_GRAPHICS_TEXTURE

#include <xd/vendor/glew/glew.h>
#include <xd/ref_counted.hpp>
#include <xd/asset_serializer.hpp>
#include <xd/graphics/image.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <memory>
#include <string>

namespace xd
{
	class XD_API texture : public xd::ref_counted, public boost::noncopyable
	{
	public:
		typedef boost::intrusive_ptr<texture> ptr;

		texture(int width, int height, const void *data = 0,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT,
			GLint mag_filter = GL_LINEAR, GLint min_filter = GL_LINEAR);
		texture(const std::string& filename,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT,
			GLint mag_filter = GL_LINEAR, GLint min_filter = GL_LINEAR);
		texture(const xd::image& image,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT,
			GLint mag_filter = GL_LINEAR, GLint min_filter = GL_LINEAR);
		virtual ~texture();

		void bind() const;
		void bind(int unit) const;

		void load(const std::string& filename);
		void load(const xd::image& image);
		void load(int width, int height, const void *data);
		void load(const void *data);

		GLuint texture_id() const;
		int width() const;
		int height() const;

		void set_wrap(GLint wrap_s, GLint wrap_t);
		void set_filter(GLint mag_filter, GLint min_filter);

	private:
		GLuint m_texture_id;
		int m_width;
		int m_height;

		void init();
	};

	template <>
	struct asset_serializer<xd::texture>
	{
		typedef std::string key_type;
		key_type operator()(const std::string& filename,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT,
			GLint mag_filter = GL_LINEAR, GLint min_filter = GL_LINEAR) const
		{
			return filename;
		}
		key_type operator()(const xd::image& image,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT,
			GLint mag_filter = GL_LINEAR, GLint min_filter = GL_LINEAR) const
		{
			return image.filename();
		}
	};
}

#endif
