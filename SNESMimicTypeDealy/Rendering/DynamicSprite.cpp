#include "DynamicSprite.h"
#include "../Rendering/Renderers/Renderer.h"

namespace GAME_NAME
{

	namespace Rendering
	{
#pragma region Constructors


		DynamicSprite::DynamicSprite(Sprite& sprite)
			: DynamicSprite(sprite.GetSpriteId())
		{

		}

		DynamicSprite::DynamicSprite(const unsigned int spriteId)
			: Sprite(spriteId), m_textureCoords{ Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f) }, m_textureColor{ Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f) }, m_vertices{ Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f) }
		{
			
		}

		DynamicSprite::DynamicSprite(const unsigned int spriteId, Vec2 vertices[4])
			: Sprite(spriteId), m_textureCoords{ Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f) }, m_textureColor{ Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f) }, m_vertices{ vertices[0], vertices[1], vertices[2], vertices[3] }
		{

		}

		DynamicSprite::DynamicSprite(const unsigned int spriteId, Vec2 vertices[4], Vec2 textureCoords[4])
			: Sprite(spriteId), m_textureCoords{ textureCoords[0], textureCoords[1], textureCoords[2], textureCoords[3] }, m_textureColor{ Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f) }, m_vertices{ vertices[0], vertices[1], vertices[2], vertices[3] }
		{

		}
		
		DynamicSprite::DynamicSprite(const unsigned int spriteId, Vec2 vertices[4], Vec2 textureCoords[4], Vec4 textureColor[4])
			: Sprite(spriteId), m_textureCoords{ textureCoords[0], textureCoords[1], textureCoords[2], textureCoords[3] }, m_textureColor{ textureColor[0], textureColor[1], textureColor[2], textureColor[3] }, m_vertices{ vertices[0], vertices[1], vertices[2], vertices[3] }
		{

		}

		DynamicSprite::~DynamicSprite()
		{
			//delete[] m_vertices;
			//delete[] m_textureCoords;
			//delete[] m_textureColor;
		}

		void DynamicSprite::UpdateVertices(const Vec2 verts[4])
		{
			m_vertices[0] = verts[0];
			m_vertices[1] = verts[1];
			m_vertices[2] = verts[2];
			m_vertices[3] = verts[3];
		}

		void DynamicSprite::UpdateTextureVerticies(const Vec2 texVerts[4])
		{
			m_textureCoords[0] = texVerts[0];
			m_textureCoords[1] = texVerts[1];
			m_textureCoords[2] = texVerts[2];
			m_textureCoords[3] = texVerts[3];
		}

		void DynamicSprite::UpdateTextureColor(const Vec4 textureColor[4])
		{
			m_textureColor[0] = textureColor[0];
			m_textureColor[1] = textureColor[1];
			m_textureColor[2] = textureColor[2];
			m_textureColor[3] = textureColor[3];
		}

#pragma endregion


		void DynamicSprite::Render(const Vec2 cameraPosition, Vec2 position, Vec2 scale, float rotation)
		{
			glBindTexture(GL_TEXTURE_2D, Renderer::GetTextureIDFromIndex(m_spriteId));
			glLoadIdentity();
			glTranslatef((position.X - cameraPosition.X) * m_resolutionScale.X, (position.Y - cameraPosition.Y) * m_resolutionScale.Y, 0);

			glScalef(scale.X * m_resolutionScale.X, scale.Y * m_resolutionScale.Y, 1.f);

			glRotatef(rotation, 0, 0, 1);

			glBegin(GL_QUADS);

			glTexCoord2f(m_textureCoords[0].X, m_textureCoords[0].Y);
			glColor4f(m_textureColor[0].X, m_textureColor[0].Y, m_textureColor[0].Z, m_textureColor[0].W);
			glVertex2f(m_vertices[0].X, m_vertices[0].Y);

			glTexCoord2f(m_textureCoords[1].X, m_textureCoords[1].Y);
			glColor4f(m_textureColor[1].X, m_textureColor[1].Y, m_textureColor[1].Z, m_textureColor[1].W);
			glVertex2f(m_vertices[1].X, m_vertices[1].Y);

			glTexCoord2f(m_textureCoords[2].X, m_textureCoords[2].Y);
			glColor4f(m_textureColor[2].X, m_textureColor[2].Y, m_textureColor[2].Z, m_textureColor[2].W);
			glVertex2f(m_vertices[2].X, m_vertices[2].Y);

			glTexCoord2f(m_textureCoords[3].X, m_textureCoords[3].Y);
			glColor4f(m_textureColor[3].X, m_textureColor[3].Y, m_textureColor[3].Z, m_textureColor[3].W);
			glVertex2f(m_vertices[3].X, m_vertices[3].Y);

			glColor4f(1.f, 1.f, 1.f, 1.f);
			glColor4f(1.f, 1.f, 1.f, 1.f);
			glColor4f(1.f, 1.f, 1.f, 1.f);
			glColor4f(1.f, 1.f, 1.f, 1.f);

			glEnd();
		}

	}
}