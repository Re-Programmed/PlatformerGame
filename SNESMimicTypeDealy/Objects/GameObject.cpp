#include "GameObject.h"
#include "../Utils/Math/VMath.h"

namespace GAME_NAME
{

    namespace Objects
    {

        void GameObject::SetPosition(Vec2 position)
        {
            m_position = position;
        }

        Vec2 GameObject::GetPosition()
        {
            return m_position;
        }

        void GameObject::Translate(const Vec2& translate)
        {
            SetPosition(m_position + translate);
        }

        void GameObject::Translate(const float& x, const float& y)
        {
            m_position.X += x;
            m_position.Y += y;
        }

        void GameObject::SetPositionAndScale(Vec2& position, Vec2& scale, std::shared_ptr<Rendering::Sprite> sprite)
        {
            m_position = position;
            m_scale = scale;
            if (sprite != nullptr) { m_sprite = sprite; }
        }

        void GameObject::SetScale(Vec2 scale)
        {
            m_scale = scale;
        }

        Vec2 GameObject::GetScale()
        {
            return m_scale;
        }

        float GameObject::GetRotation()
        {
            return m_rotation;
        }

        void GameObject::RotateAboutCenter(float rotation)
        {
            //Get the rotation of the object before it has rotated starting from the bottom left in radians. (Subtract 3pi/4 to make the rotation begin from the bottom left.)
            float rotOffPrev = MathUtils::to_radf(m_rotation) - M_3_PI_4f;

            //Rotate the object from the bottom left.
            m_rotation += rotation;

            if (m_rotation >= 360)
            {
                m_rotation -= 360;
            }

            if (m_rotation <= -360)
            {
                m_rotation += 360;
            }
            
            //Calculate the new rotation in radians by adding the rotation.
            float rotOff = rotOffPrev + MathUtils::to_radf(rotation);

            //Translate the object by calculating the distance between the desired center and the new center based on the distance the center traveled (ex. cos(start) - cos(prev) * [diagonal midpoint]).
            m_rotationOffset += Vec2((m_scale.X * MATHUTILS_INVSQRT2f) * (std::cosf(rotOff) - std::cosf(rotOffPrev)), (m_scale.Y * MATHUTILS_INVSQRT2f) * (std::sinf(rotOff) - std::sinf(rotOffPrev)));
        }

        void GameObject::SetRotationAboutCenter(float rotation)
        {
            RotateAboutCenter(rotation - m_rotation);
        }

        const std::shared_ptr<Rendering::Sprite> GameObject::GetSprite()
        {
            return m_sprite;
        }

        void GameObject::SetSprite(Rendering::Sprite* sprite)
        {
            //delete m_sprite;
            m_sprite.reset(sprite);
            //m_sprite = std::shared_ptr<Rendering::Sprite>(sprite);
        }

        void GameObject::SetSprite(std::shared_ptr<Rendering::Sprite> sprite)
        {
            m_sprite = sprite;
        }

        void GameObject::SetSpriteId(const unsigned int& ID)
        {
            m_sprite->SetSpriteId(ID);
        }


        //Default constructor.
        GameObject::GameObject() : m_position(Vec2::Zero), m_scale(Vec2::Zero), m_rotation(0.f), m_rotationOffset(0), m_sprite(nullptr), m_textureFlipped(false)
        {

        }

        GameObject::~GameObject()
        {
            //UH OH MEMORY LEAK? WHY NO DELETE FIX FIX FIX (TODO FIX)
            //delete m_sprite;
        }

        void GameObject::Render(const Vec2& cameraPosition)
        {
            //It is possible for a game object to have no texture.
            if (m_sprite != nullptr)
            {
                m_sprite->Render(cameraPosition, m_rotationOffset + m_position + (m_textureFlipped ? (m_scale * Vec2::OneX) : 0), m_scale * (m_textureFlipped ? Vec2::MinusOneX : 1), m_rotation);
            }
        }

    }
}