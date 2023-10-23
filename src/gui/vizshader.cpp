#include <spiralviz/gui/vizshader.hpp>

#include <GL/gl.h>

VizShader::VizShader(const VizPaths& paths)
{
    reload_shader_from_paths(paths.frag_path, paths.vert_path);
    reload_colormap_from_path(paths.colormap_path);
}

void VizShader::render_into(sf::RenderTarget& target, sf::FloatRect target_rect)
{
    reload_uniforms();

    m_shader.setUniform("resolution", sf::Glsl::Vec2{target_rect.width, target_rect.height});

    sf::RectangleShape target_shape{{target_rect.width, target_rect.height}};
    target_shape.setPosition(target_rect.left, target_rect.top);

    target.draw(target_shape, &m_shader);
}

void VizShader::render_into(sf::RenderTarget& target)
{
    const sf::Vector2u target_size = target.getSize();
    const sf::FloatRect target_rect {
        {0.0f, 0.0f},
        {float(target_size.x), float(target_size.y)}
    };
    render_into(target, target_rect);
}

void VizShader::update_fft_texture(std::span<const float> fft_data, std::size_t sample_rate)
{
    m_sample_rate = sample_rate;

    if (m_fft.getSize().x != fft_data.size())
    {
        m_fft.create(fft_data.size(), 1);
        m_fft.setSmooth(true);
    }

    assert(fft_data.size() < m_fft.getMaximumSize());

    sf::Texture::bind(&m_fft);
    // this is very silly, but it does work, so it's fine as long as SFML
    // doesn't try to poke at the texture which it shouldn't if we don't use its
    // API directly.
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R32F,
        fft_data.size(),
        1,
        0,
        GL_RED,
        GL_FLOAT,
        fft_data.data()
    );
    sf::Texture::bind(nullptr);

}

void VizShader::reload_shader_from_paths(const char* frag_path, const char* vert_path)
{
    if (!m_shader.loadFromFile(vert_path, sf::Shader::Vertex))
    {
        throw std::runtime_error("Failed to compile vertex shader");
    }

    if (!m_shader.loadFromFile(frag_path, sf::Shader::Fragment))
    {
        throw std::runtime_error("Failed to compile fragment shader");
    }
}

void VizShader::reload_colormap_from_path(const char* colormap_path)
{
    if (!m_colormap.loadFromFile(colormap_path))
    {
        throw std::runtime_error("Failed to load colormap texture");
    }

    m_colormap.setSmooth(true);
}

void VizShader::reload_uniforms()
{
    m_shader.setUniform("fft", m_fft);
    m_shader.setUniform("fft_size", int(m_fft.getSize().x));
    m_shader.setUniform("sample_rate", float(m_sample_rate));

    m_shader.setUniform("spiral_dis", m_params.spiral_dis);
    m_shader.setUniform("spiral_width", m_params.spiral_width);
    m_shader.setUniform("spiral_blur", m_params.spiral_blur);
    m_shader.setUniform("vol_min", m_params.vol_min);
    m_shader.setUniform("vol_max", m_params.vol_max);

    m_shader.setUniform("cmap", m_colormap);
}
