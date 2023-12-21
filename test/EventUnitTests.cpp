
#include <Events/ApplicationEvent.h>
#include <Events/Event.h>
#include <Events/KeyEvent.h>
#include <Events/MouseEvent.h>

#include <gtest/gtest.h>

TEST(CoreTest, WindowResizeEvent)
{
    Core::WindowResizeEvent event(800, 600);
    // Expect equality.
    EXPECT_EQ(event.GetCategoryFlags(), Core::EventCategory::EventCategoryApplication);
    EXPECT_EQ(event.GetEventType(), Core::EventType::WindowResize);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "WindowResizeEvent: 800, 600");
}

TEST(CoreTest, WindowCloseEvent)
{
    Core::WindowCloseEvent event;
    // Expect equality.
    EXPECT_EQ(event.GetCategoryFlags(), Core::EventCategory::EventCategoryApplication);
    EXPECT_EQ(event.GetEventType(), Core::EventType::WindowClose);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "WindowClose");
}

TEST(CoreTest, AppTickEvent)
{
    Core::AppTickEvent event;
    // Expect equality.
    EXPECT_EQ(event.GetCategoryFlags(), Core::EventCategory::EventCategoryApplication);
    EXPECT_EQ(event.GetEventType(), Core::EventType::AppTick);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "AppTick");
}

TEST(CoreTest, AppUpdateEvent)
{
    Core::AppUpdateEvent event;
    // Expect equality.
    EXPECT_EQ(event.GetCategoryFlags(), Core::EventCategory::EventCategoryApplication);
    EXPECT_EQ(event.GetEventType(), Core::EventType::AppUpdate);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "AppUpdate");
}

TEST(CoreTest, AppRenderEvent)
{
    Core::AppRenderEvent event;
    // Expect equality.
    EXPECT_EQ(event.GetCategoryFlags(), Core::EventCategory::EventCategoryApplication);
    EXPECT_EQ(event.GetEventType(), Core::EventType::AppRender);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "AppRender");
}

TEST(CoreTest, IsInCategory)
{
    Core::AppRenderEvent event;
    // Expect equality.
    EXPECT_EQ(event.GetCategoryFlags(), Core::EventCategory::EventCategoryApplication);
    EXPECT_EQ(event.GetEventType(), Core::EventType::AppRender);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "AppRender");
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryApplication), true);
}

TEST(CoreTest,
     EventDispatcherfalse) // Dispatch returns true when the function is called if the event is handeled is irrelavant
{
    Core::WindowResizeEvent event(800, 600);
    Core::EventDispatcher dispatcher(event);

    // Expect equality.
    EXPECT_EQ(dispatcher.Dispatch<Core::WindowResizeEvent>([](Core::WindowResizeEvent &e) { return false; }), true);
}

TEST(CoreTest, EventDispatchertrue)
{
    Core::WindowResizeEvent event(800, 600);
    Core::EventDispatcher dispatcher(event);

    // Expect equality.
    EXPECT_EQ(dispatcher.Dispatch<Core::WindowResizeEvent>([](Core::WindowResizeEvent &e) { return true; }), true);
}

TEST(CoreTest, EventDispatcherFunctionNotDefined)
{
    Core::WindowCloseEvent event;
    Core::EventDispatcher dispatcher(event);

    // Expect equality.
    EXPECT_EQ(dispatcher.Dispatch<Core::WindowResizeEvent>([](Core::WindowResizeEvent &e) { return true; }), false);
}

TEST(CoreTest, EventDispatcherHandledtrue) // but the return of the called function does matter to handeled
{
    Core::WindowResizeEvent event(800, 600);
    Core::EventDispatcher dispatcher(event);

    // Expect equality.
    EXPECT_EQ(dispatcher.Dispatch<Core::WindowResizeEvent>([](Core::WindowResizeEvent &e) { return true; }), true);
    EXPECT_EQ(event.Handled, true);
}

TEST(CoreTest, EventDispatcherHandledfalse)
{
    Core::WindowResizeEvent event(800, 600);
    Core::EventDispatcher dispatcher(event);

    // Expect equality.
    EXPECT_EQ(dispatcher.Dispatch<Core::WindowResizeEvent>([](Core::WindowResizeEvent &e) { return false; }), true);
    EXPECT_EQ(event.Handled, false);
}

TEST(CoreTest, KeyEvent)
{
    Core::KeyPressedEvent event(Core::Key::T);
    // Expect equality.
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryKeyboard), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryInput), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryMouseButton), false);
    EXPECT_EQ(event.GetEventType(), Core::EventType::KeyPressed);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "KeyPressedEvent: 84 (repeat = 0)");
    EXPECT_EQ(event.IsRepeat(), false);
}

TEST(CoreTest, KeyEventIsRepeat)
{
    Core::KeyPressedEvent event(Core::Key::T, true);
    // Expect equality.
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryKeyboard), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryInput), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryApplication), false);
    EXPECT_EQ(event.GetEventType(), Core::EventType::KeyPressed);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "KeyPressedEvent: 84 (repeat = 1)");
    EXPECT_EQ(event.IsRepeat(), true);
}

TEST(CoreTest, KeyReleasedEvent)
{
    Core::KeyReleasedEvent event(Core::Key::T);
    // Expect equality.
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryKeyboard), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryInput), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryApplication), false);
    EXPECT_EQ(event.GetEventType(), Core::EventType::KeyReleased);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "KeyReleasedEvent: 84");
}

TEST(CoreTest, KeyTypedEvent)
{
    Core::KeyTypedEvent event(Core::Key::T);
    // Expect equality.
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryKeyboard), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryInput), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryApplication), false);
    EXPECT_EQ(event.GetEventType(), Core::EventType::KeyTyped);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "KeyTypedEvent: 84");
}

TEST(CoreTest, MouseMovedEvent)
{
    Core::MouseMovedEvent event(1, 2);
    // Expect equality.
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryMouse), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryInput), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryApplication), false);
    EXPECT_EQ(event.GetEventType(), Core::EventType::MouseMoved);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "MouseMovedEvent: 1, 2");
    EXPECT_EQ(event.GetX(), 1);
    EXPECT_EQ(event.GetY(), 2);
}

TEST(CoreTest, MouseScrolledEvent)
{
    Core::MouseScrolledEvent event(1, 2);
    // Expect equality.
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryMouse), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryInput), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryApplication), false);
    EXPECT_EQ(event.GetEventType(), Core::EventType::MouseScrolled);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "MouseScrolledEvent: 1, 2");
    EXPECT_EQ(event.GetXOffset(), 1);
    EXPECT_EQ(event.GetYOffset(), 2);
}

TEST(CoreTest, MouseButtonPressedEvent)
{
    Core::MouseButtonPressedEvent event(Core::Mouse::Button0);
    // Expect equality.
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryMouse), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryInput), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryApplication), false);
    EXPECT_EQ(event.GetEventType(), Core::EventType::MouseButtonPressed);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "MouseButtonPressedEvent: 0");
    EXPECT_EQ(event.GetMouseButton(), Core::Mouse::Button0);
}

TEST(CoreTest, MouseButtonReleasedEvent)
{
    Core::MouseButtonReleasedEvent event(Core::Mouse::Button0);
    // Expect equality.
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryMouse), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryInput), true);
    EXPECT_EQ(event.IsInCategory(Core::EventCategory::EventCategoryApplication), false);
    EXPECT_EQ(event.GetEventType(), Core::EventType::MouseButtonReleased);
    EXPECT_EQ(event.Handled, false);
    EXPECT_EQ(event.ToString(), "MouseButtonReleasedEvent: 0");
    EXPECT_EQ(event.GetMouseButton(), Core::Mouse::Button0);
}
