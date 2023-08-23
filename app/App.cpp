#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <Core/Shader.hpp>
#include <Core/Camera.hpp>

void displayGpuInfo()
{
    std::cout
        << "---------------------------------------------\n"
        << "Vendor:  \t" << glGetString(GL_VENDOR) <<   '\n'
        << "Version: \t" << glGetString(GL_VERSION) <<  '\n'
        << "Renderer:\t" << glGetString(GL_RENDERER) << '\n'
        << "---------------------------------------------\n";
}

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    sf::Window window(sf::VideoMode(800, 600), "OpenGL Workbench", sf::Style::Default, settings);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction)))
    {
        std::cout << "Failed to initialize GLAD!\n";
        return EXIT_FAILURE;
    }

    displayGpuInfo();

    Shader shader;
    shader.loadFromFile("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl");

    Camera camera;

    std::vector<GLfloat> vertices
    {
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(window.getSize().x) / window.getSize().y,
        0.1f,
        100.0f
    );

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::W: camera.ProcessKeyboard(CameraMovement::Forward, 1.0f); break;
                    case sf::Keyboard::A: camera.ProcessKeyboard(CameraMovement::Left, 1.0f); break;
                    case sf::Keyboard::S: camera.ProcessKeyboard(CameraMovement::Backward, 1.0f); break;
                    case sf::Keyboard::D: camera.ProcessKeyboard(CameraMovement::Right, 1.0f); break;
                    case sf::Keyboard::Escape: window.close(); break;
                    default: break;
                }
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setMat4("Model", model);
        shader.setMat4("View", camera.GetViewMatrix());
        shader.setMat4("Projection", projection);

        glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        window.display();
    }

    return EXIT_SUCCESS;
}