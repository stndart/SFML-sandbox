#include "Scene.h"

Scene::Scene(std::string name) : name(name)
{

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

void Scene::addSprite(AnimatedSprite* sprite)
{
    sprites.push_back(sprite);
}

void Scene::update(Event& event)
{
    if (event.type == Event::MouseButtonPressed){
        switch (event.mouseButton.button)
        {
        case Mouse::Left:
            for (auto s : sprites)
            {
                ///FloatRect curRect = s->getGlobalBounds();
                ///Vector2f curPos = Vector2f(Mouse::getPosition());
                ///if (curPos.x >= curRect.left && curPos.x <= (curRect.left + curRect.width)
                ///    && curPos.y >= curRect.top && curPos.x <= (curRect.top + curRect.height))

                Vector2f curPos = Vector2f(event.mouseButton.x, event.mouseButton.y);

                /*std::cout << event.mouseButton.x << " - " << event.mouseButton.y << std::endl;
                std::cout << curPos.x << " " << curPos.y << std::endl;
                std::cout << s->getGlobalBounds().left << "x" << s->getGlobalBounds().top << std::endl;
                std::cout << s->getGlobalBounds().contains(curPos) << std::endl;//*/

                if (s->getGlobalBounds().contains(curPos))
                {
                    s->onClick(true);
                }
            }
            break;

        default:
            break;
        }
    }
    if (event.type == Event::MouseButtonReleased)
    {
        switch (event.mouseButton.button)
        {
        case Mouse::Left:
            for (auto s : sprites)
            {

                ///FloatRect curRect = s->getGlobalBounds();
                ///Vector2f curPos = Vector2f(Mouse::getPosition());
                ///if (curPos.x >= curRect.left && curPos.x <= (curRect.left + curRect.width)
                ///    && curPos.y >= curRect.top && curPos.x <= (curRect.top + curRect.height))
                Vector2f curPos = Vector2f(event.mouseButton.x, event.mouseButton.y);

                /*std::cout << event.mouseButton.x << " - " << event.mouseButton.y << std::endl;
                std::cout << curPos.x << " " << curPos.y << std::endl;
                std::cout << s->getGlobalBounds().left << "x" << s->getGlobalBounds().top << std::endl;
                std::cout << s->getGlobalBounds().contains(curPos) << std::endl;//*/

                if (s->getGlobalBounds().contains(curPos))
                {
                    s->onClick(false);
                }
            }
            break;

        default:
            break;
        }
    }
}

void Scene::update(Time deltaTime)
{
    for (auto s : sprites)
    {
        s->getGlobalBounds();
        deltaTime += seconds(0.00002f);
        s->update(deltaTime);
    }
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
            target.draw(*sprites[i]);
}

Scene new_menu_scene(Texture* bg, Texture* new_button, Texture* new_button_pressed, Vector2i screen_dimensions)
{
    Scene main_menu("Main menu");
    main_menu.addTexture(bg, IntRect(0, 0, 1920, 1080));
    main_menu.setScale((float)screen_dimensions.x / 1920, (float)screen_dimensions.y / 1080);

    if (new_button == new_button_pressed)
        throw;

    AnimatedSprite* button = new AnimatedSprite("asprite", *new_button, IntRect(0, 0, 1000, 500));
    button->getAnimation()->addSpriteSheet(*new_button_pressed);
    button->getAnimation()->addFrame(IntRect(0, 0, 1000, 500), 1);
    button->setLooped(false);
    button->setScale(0.5f, 0.5f);
    button->setOrigin(button->getLocalBounds().width / 2.0f,
                      button->getLocalBounds().height / 2.0f);
    button->setPosition(screen_dimensions.x / 2, screen_dimensions.y / 4 * 3);//*/


    /*Sprite* button = new Sprite(*new_button, IntRect(0, 0, 1000, 500));
    button->setScale(0.5f, 0.5f);
    button->setOrigin(button->getLocalBounds().width / 2.0f,
                      button->getLocalBounds().height / 2.0f);
    button->setPosition(screen_dimensions.x / 2, screen_dimensions.y / 4 * 3);//*/

    main_menu.addSprite(button);

    FloatRect b;
    b = button->getLocalBounds();
    std::cout << "local bounds " << b.left << " " << b.top << " | " << b.width << " " << b.height << std::endl;
    b = button->getGlobalBounds();
    std::cout << "bounds " << b.left << " " << b.top << " | " << b.width << " " << b.height << std::endl;
    Vector2f o = button->getOrigin();
    std::cout << "origin " << o.x << " " << o.y << std::endl;
    Vector2f p = button->getPosition();
    std::cout << "position " << p.x << " " << p.y << std::endl;

    return main_menu;
}
