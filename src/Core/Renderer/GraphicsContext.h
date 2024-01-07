#pragma once

namespace Core
{

class GraphicsContext
{
  public:
    virtual ~GraphicsContext() = default;

    virtual bool Init() = 0;
    virtual bool Shutdown() = 0;
    virtual void drawFrame() = 0;

    static std::unique_ptr<GraphicsContext> Create(void *window);
};

} // namespace Core
