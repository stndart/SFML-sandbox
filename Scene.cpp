#include "Scene.h"

Scene::Scene(std::string name) : name(name)
{

}

void Scene::draw(RenderTarget& target, RenderStates states) const
{
    //std::cout << "Who asked " << name << " to draw?\n";
    //std::cout << name << " draw pos " << getPosition().x << " " << getPosition().y << std::endl;

    if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }
    for (std::size_t i = 0; i < sprites.size(); ++i)
        if (sprites[i])
            target.draw(*sprites[i], states);
}

void Scene::addTexture(Texture* texture, IntRect rect)
{
    background = texture;

    m_vertices[0].position = Vector2f(0.f, 0.f);
    m_vertices[1].position = Vector2f(0.f, static_cast<float>(rect.height));
    m_vertices[2].position = Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
    m_vertices[3].position = Vector2f(static_cast<float>(rect.width), 0.f);

    float left = static_cast<float>(rect.left) + 0.0001f;
    float right = left + static_cast<float>(rect.width);
    float top = static_cast<float>(rect.top);
    float bottom = top + static_cast<float>(rect.height);

    m_vertices[0].texCoords = Vector2f(left, top);
    m_vertices[1].texCoords = Vector2f(left, bottom);
    m_vertices[2].texCoords = Vector2f(right, bottom);
    m_vertices[3].texCoords = Vector2f(right, top);
}

void Scene::addSprite(Sprite* sprite)
{
    sprites.push_back(sprite);
}

void Scene::update(Time deltaTime)
{
    // полная фигня, чтобы компиллер не ругался на неиспользуемую переменную

    if (deltaTime.asSeconds() < 0)
        return;
}

Scene new_menu_scene(Texture* bg, Texture* new_button, Vector2i screen_dimensions)
{
    Scene main_menu("Main menu");
    main_menu.addTexture(bg, IntRect(0, 0, 1920, 1080));
    //main_menu.setScale(0.6f, 0.6f);

    /*AnimatedSprite* button = new AnimatedSprite("new game button");
    Animation* new_game = new Animation();
    new_game->setSpriteSheet(*new_button);
    new_game->addFrame(IntRect(0, 0, 1000, 500));
    button->setAnimation(*new_game);
    button->setPosition(Vector2f(0, 0));*/

    Sprite* button = new Sprite(*new_button, IntRect(0, 0, 1000, 500));
    button->setScale(0.5f, 0.5f);
    button->setOrigin(button->getLocalBounds().width / 2.0f,
                      button->getLocalBounds().height / 2.0f);
    button->setPosition(screen_dimensions.x / 2, screen_dimensions.y / 4 * 3);
    main_menu.addSprite(button);

    return main_menu;
}
