#include <xd/graphics/sprite_batch.hpp>
#include <xd/graphics/texture.hpp>
#include <xd/graphics/vertex_batch.hpp>
#include <xd/graphics/shader_program.hpp>
#include <xd/graphics/utility.hpp>
#include <boost/optional.hpp>
#include <deque>

namespace xd { namespace detail {

	struct sprite
	{
		texture::ptr tex;
		rect src;
		float x, y;
		float rotation;
		xd::vec2 scale;
		xd::vec2 origin;
		xd::vec4 color;
		float depth;
	};

	struct sprite_batch_data
	{
		std::deque<sprite> sprites;
		std::unique_ptr<xd::shader_program> shader;
		sprite_batch_data() : shader(new xd::sprite_shader) {}
	};

} }

xd::sprite_batch::sprite_batch()
	: m_data(new detail::sprite_batch_data)
	, m_scale(1)
{
}

xd::sprite_batch::~sprite_batch()
{
	delete m_data;
}

void xd::sprite_batch::clear()
{
	m_data->sprites.clear();
}

xd::sprite_batch::batch_list xd::sprite_batch::create_batches()
{
	std::vector<xd::vertex_batch<xd::detail::sprite_vertex_traits>::ptr> batches;

	// create a quad for rendering sprites
	xd::detail::sprite_vertex quad[4];

	// iterate through all sprites
	for (auto i = m_data->sprites.begin(); i != m_data->sprites.end(); ++i) {
		// create a vertex batch for sending vertex data
		xd::vertex_batch<xd::detail::sprite_vertex_traits>::ptr batch(new xd::vertex_batch<xd::detail::sprite_vertex_traits>(GL_QUADS));
		// so we need to type less ;)
		auto tw = i->tex->width();
		auto th = i->tex->height();
		auto& src = i->src;
		auto& origin = i->origin;

		// calculate scale
		vec2 scale = m_scale * i->scale;

		// assign position
		quad[0].pos = vec2(scale.x*(0-origin.x)*src.w, scale.y*(1-origin.y)*src.h);
		quad[1].pos = vec2(scale.x*(1-origin.x)*src.w, scale.y*(1-origin.y)*src.h);
		quad[2].pos = vec2(scale.x*(1-origin.x)*src.w, scale.y*(0-origin.y)*src.h);
		quad[3].pos = vec2(scale.x*(0-origin.x)*src.w, scale.y*(0-origin.y)*src.h);

		// if there's rotation
		if (i->rotation) {
			// construct a rotation matrix
			auto rotation_matrix = rotate(mat4(), i->rotation, vec3(0, 0, 1));
			// rotate the 4 vertices
			quad[0].pos = vec2(rotation_matrix * vec4(quad[0].pos, 0, 1));
			quad[1].pos = vec2(rotation_matrix * vec4(quad[1].pos, 0, 1));
			quad[2].pos = vec2(rotation_matrix * vec4(quad[2].pos, 0, 1));
			quad[3].pos = vec2(rotation_matrix * vec4(quad[3].pos, 0, 1));
		}

		// assign texture pos
		quad[0].texpos = vec2(src.x/tw        , (src.y+src.h)/th);
		quad[1].texpos = vec2((src.x+src.w)/tw, (src.y+src.h)/th);
		quad[2].texpos = vec2((src.x+src.w)/tw, src.y/th);
		quad[3].texpos = vec2(src.x/tw        , src.y/th);

		// load the quad
		batch->load(&quad[0], 4);
		batches.push_back(batch);

	}
	
	return batches;
}

void xd::sprite_batch::draw(const xd::mat4& mvp_matrix, const xd::sprite_batch::batch_list& batches, int ticks)
{
	assert(m_data->sprites.size() == batches.size());
	// setup the shader
	m_data->shader->use();
	m_data->shader->bind_uniform("mvpMatrix", mvp_matrix);
	m_data->shader->bind_uniform("ticks", ticks);

	// iterate through all sprites
	for (unsigned int i = 0; i < m_data->sprites.size(); ++i) {
		auto& sprite = m_data->sprites[i];
		auto& batch = batches[i];
		// give required params to shader
		m_data->shader->bind_uniform("vPosition", vec4(sprite.x, sprite.y, sprite.depth, 0));
		m_data->shader->bind_uniform("vColor", sprite.color);
		m_data->shader->bind_uniform("vColorKey", sprite.tex->color_key());

		// bind the texture
		sprite.tex->bind(GL_TEXTURE0);
		m_data->shader->bind_uniform("vTexSize", vec2(sprite.tex->width(), sprite.tex->height()));
		
		// draw it
		batch->render();
	}
}

void xd::sprite_batch::draw(const xd::mat4& mvp_matrix, int ticks)
{
	// create a vertex batch for sending vertex data
	vertex_batch<detail::sprite_vertex_traits> batch(GL_QUADS);

	// setup the shader
	m_data->shader->use();
	m_data->shader->bind_uniform("mvpMatrix", mvp_matrix);
	m_data->shader->bind_uniform("ticks", ticks);

	// create a quad for rendering sprites
	detail::sprite_vertex quad[4];

	// iterate through all sprites
	for (auto i = m_data->sprites.begin(); i != m_data->sprites.end(); ++i) {
		// so we need to type less ;)
		auto tw = i->tex->width();
		auto th = i->tex->height();
		auto& src = i->src;
		auto& origin = i->origin;

		// calculate scale
		vec2 scale = m_scale * i->scale;

		// assign position
		quad[0].pos = vec2(scale.x*(0-origin.x)*src.w, scale.y*(1-origin.y)*src.h);
		quad[1].pos = vec2(scale.x*(1-origin.x)*src.w, scale.y*(1-origin.y)*src.h);
		quad[2].pos = vec2(scale.x*(1-origin.x)*src.w, scale.y*(0-origin.y)*src.h);
		quad[3].pos = vec2(scale.x*(0-origin.x)*src.w, scale.y*(0-origin.y)*src.h);

		// if there's rotation
		if (i->rotation) {
			// construct a rotation matrix
			auto rotation_matrix = rotate(mat4(), i->rotation, vec3(0, 0, 1));
			// rotate the 4 vertices
			quad[0].pos = vec2(rotation_matrix * vec4(quad[0].pos, 0, 1));
			quad[1].pos = vec2(rotation_matrix * vec4(quad[1].pos, 0, 1));
			quad[2].pos = vec2(rotation_matrix * vec4(quad[2].pos, 0, 1));
			quad[3].pos = vec2(rotation_matrix * vec4(quad[3].pos, 0, 1));
		}

		// assign texture pos
		quad[0].texpos = vec2(src.x/tw        , (src.y+src.h)/th);
		quad[1].texpos = vec2((src.x+src.w)/tw, (src.y+src.h)/th);
		quad[2].texpos = vec2((src.x+src.w)/tw, src.y/th);
		quad[3].texpos = vec2(src.x/tw        , src.y/th);

		// load the quad
		batch.load(&quad[0], 4);
		
		// give required params to shader
		m_data->shader->bind_uniform("vPosition", vec4(i->x, i->y, i->depth, 0));
		m_data->shader->bind_uniform("vColor", i->color);
		m_data->shader->bind_uniform("vColorKey", i->tex->color_key());

		// bind the texture
		i->tex->bind(GL_TEXTURE0);
		m_data->shader->bind_uniform("vTexSize", vec2(i->tex->width(), i->tex->height()));
		
		// draw it
		batch.render();
	}
}

void xd::sprite_batch::set_scale(float scale)
{
	m_scale = scale;
}

float xd::sprite_batch::get_scale() const
{
	return m_scale;
}

void xd::sprite_batch::set_shader(shader_program* shader) {
	m_data->shader.reset(shader);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, float x, float y,
		const xd::vec4& color, const xd::vec2& origin)
{
	add(texture, rect(0, 0, texture->width(), texture->height()), x, y, 0,
		vec2(1, 1), color, origin);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, float x, float y,
		float rotation, float scale, const xd::vec4& color, const xd::vec2& origin)
{
	add(texture, rect(0, 0, texture->width(), texture->height()),
		x, y, rotation, vec2(scale, scale), color, origin);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, float x, float y,
		float rotation, const xd::vec2& scale,
		const xd::vec4& color, const xd::vec2& origin)
{
	add(texture, rect(0, 0, texture->width(), texture->height()),
		x, y, rotation, scale,color, origin);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, const xd::rect& src, float x, float y,
		const xd::vec4& color, const xd::vec2& origin)
{
	add(texture, src, x, y, 0, vec2(1, 1), color, origin);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, const xd::rect& src,
		float x, float y, float rotation, float scale,
		const xd::vec4& color, const xd::vec2& origin)
{
	add(texture, src, x, y, rotation, vec2(scale, scale), color, origin);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, const xd::rect& src,
		float x, float y, float rotation, const xd::vec2& scale,
		const xd::vec4& color, const xd::vec2& origin)
{
	detail::sprite sprite;
	sprite.tex = texture;
	sprite.src = src;
	sprite.x = x;
	sprite.y = y;
	sprite.rotation = rotation;
	sprite.scale = scale;
	sprite.origin = origin;
	sprite.color = color;
	sprite.depth = 0.0f;
	m_data->sprites.push_back(sprite);
}
