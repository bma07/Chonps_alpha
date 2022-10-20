#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <sstream>
#include <functional>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "glm/glm.hpp"

// Platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#undef CHONPS_PLATFORM_WINDOWS
	#define CHONPS_PLATFORM_WINDOWS
#ifdef CHONPS_PLATFORM_WINDOWS
	#ifdef CHONPS_SHARED_LIBRARY
		#define CHONPS_API __declspec(dllexport)
		#define CHONPS_API_IMPORT __declspec(dllimport)
	#else 
		#define CHONPS_API
	#endif
#endif

#elif __APPLE__
	#define CHONPS_PLATFORM_APPLE

#elif __linux__
	#define CHONPS_PLATFORM_LINUX

#else
	#error "Chonps does not support the current platform."
#endif

// Window Plaform API
#ifdef CHONPS_USE_WIN32
	#define CHONPS_WIN32_API
#else
	#define CHONPS_GLFW_API
#endif

// Graphics Platform API
#ifdef CHONPS_USE_VULKAN
	#define CHONPS_VULKAN_API
#elif CHONPS_DIRECTX_API
	#define CHONPS_DIRECTX_API
#else
	#define CHONPS_OPENGL_API
#endif


// Debug
#ifdef CHONPS_DEBUG
	#define CHONPS_ENABLE_ASSERTS 
#endif

#ifdef CHONPS_ENABLE_ASSERTS
	#define CHONPS_ASSERT(x, ...) { if(!(x)) { CHONPS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define CHONPS_CORE_ASSERT(x, ...) { if(!(x)) { CHONPS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define CHONPS_ASSERT(x, ...)
	#define CHONPS_CORE_ASSERT(x, ...)
#endif

// Key Codes
#define CHONPS_KEY_SPACE              32
#define CHONPS_KEY_APOSTROPHE         39  /* ' */
#define CHONPS_KEY_COMMA              44  /* , */
#define CHONPS_KEY_MINUS              45  /* - */
#define CHONPS_KEY_PERIOD             46  /* . */
#define CHONPS_KEY_SLASH              47  /* / */
#define CHONPS_KEY_0                  48
#define CHONPS_KEY_1                  49
#define CHONPS_KEY_2                  50
#define CHONPS_KEY_3                  51
#define CHONPS_KEY_4                  52
#define CHONPS_KEY_5                  53
#define CHONPS_KEY_6                  54
#define CHONPS_KEY_7                  55
#define CHONPS_KEY_8                  56
#define CHONPS_KEY_9                  57
#define CHONPS_KEY_SEMICOLON          59  /* ; */
#define CHONPS_KEY_EQUAL              61  /* = */
#define CHONPS_KEY_A                  65
#define CHONPS_KEY_B                  66
#define CHONPS_KEY_C                  67
#define CHONPS_KEY_D                  68
#define CHONPS_KEY_E                  69
#define CHONPS_KEY_F                  70
#define CHONPS_KEY_G                  71
#define CHONPS_KEY_H                  72
#define CHONPS_KEY_I                  73
#define CHONPS_KEY_J                  74
#define CHONPS_KEY_K                  75
#define CHONPS_KEY_L                  76
#define CHONPS_KEY_M                  77
#define CHONPS_KEY_N                  78
#define CHONPS_KEY_O                  79
#define CHONPS_KEY_P                  80
#define CHONPS_KEY_Q                  81
#define CHONPS_KEY_R                  82
#define CHONPS_KEY_S                  83
#define CHONPS_KEY_T                  84
#define CHONPS_KEY_U                  85
#define CHONPS_KEY_V                  86
#define CHONPS_KEY_W                  87
#define CHONPS_KEY_X                  88
#define CHONPS_KEY_Y                  89
#define CHONPS_KEY_Z                  90
#define CHONPS_KEY_LEFT_BRACKET       91  /* [ */
#define CHONPS_KEY_BACKSLASH          92  /* \ */
#define CHONPS_KEY_RIGHT_BRACKET      93  /* ] */
#define CHONPS_KEY_GRAVE_ACCENT       96  /* ` */
#define CHONPS_KEY_WORLD_1            161 /* non-US #1 */
#define CHONPS_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define CHONPS_KEY_ESCAPE             256
#define CHONPS_KEY_ENTER              257
#define CHONPS_KEY_TAB                258
#define CHONPS_KEY_BACKSPACE          259
#define CHONPS_KEY_INSERT             260
#define CHONPS_KEY_DELETE             261
#define CHONPS_KEY_RIGHT              262
#define CHONPS_KEY_LEFT               263
#define CHONPS_KEY_DOWN               264
#define CHONPS_KEY_UP                 265
#define CHONPS_KEY_PACHONPS_UP            266
#define CHONPS_KEY_PACHONPS_DOWN          267
#define CHONPS_KEY_HOME               268
#define CHONPS_KEY_END                269
#define CHONPS_KEY_CAPS_LOCK          280
#define CHONPS_KEY_SCROLL_LOCK        281
#define CHONPS_KEY_NUM_LOCK           282
#define CHONPS_KEY_PRINT_SCREEN       283
#define CHONPS_KEY_PAUSE              284
#define CHONPS_KEY_F1                 290
#define CHONPS_KEY_F2                 291
#define CHONPS_KEY_F3                 292
#define CHONPS_KEY_F4                 293
#define CHONPS_KEY_F5                 294
#define CHONPS_KEY_F6                 295
#define CHONPS_KEY_F7                 296
#define CHONPS_KEY_F8                 297
#define CHONPS_KEY_F9                 298
#define CHONPS_KEY_F10                299
#define CHONPS_KEY_F11                300
#define CHONPS_KEY_F12                301
#define CHONPS_KEY_F13                302
#define CHONPS_KEY_F14                303
#define CHONPS_KEY_F15                304
#define CHONPS_KEY_F16                305
#define CHONPS_KEY_F17                306
#define CHONPS_KEY_F18                307
#define CHONPS_KEY_F19                308
#define CHONPS_KEY_F20                309
#define CHONPS_KEY_F21                310
#define CHONPS_KEY_F22                311
#define CHONPS_KEY_F23                312
#define CHONPS_KEY_F24                313
#define CHONPS_KEY_F25                314
#define CHONPS_KEY_KP_0               320
#define CHONPS_KEY_KP_1               321
#define CHONPS_KEY_KP_2               322
#define CHONPS_KEY_KP_3               323
#define CHONPS_KEY_KP_4               324
#define CHONPS_KEY_KP_5               325
#define CHONPS_KEY_KP_6               326
#define CHONPS_KEY_KP_7               327
#define CHONPS_KEY_KP_8               328
#define CHONPS_KEY_KP_9               329
#define CHONPS_KEY_KP_DECIMAL         330
#define CHONPS_KEY_KP_DIVIDE          331
#define CHONPS_KEY_KP_MULTIPLY        332
#define CHONPS_KEY_KP_SUBTRACT        333
#define CHONPS_KEY_KP_ADD             334
#define CHONPS_KEY_KP_ENTER           335
#define CHONPS_KEY_KP_EQUAL           336
#define CHONPS_KEY_LEFT_SHIFT         340
#define CHONPS_KEY_LEFT_CONTROL       341
#define CHONPS_KEY_LEFT_ALT           342
#define CHONPS_KEY_LEFT_SUPER         343
#define CHONPS_KEY_RIGHT_SHIFT        344
#define CHONPS_KEY_RIGHT_CONTROL      345
#define CHONPS_KEY_RIGHT_ALT          346
#define CHONPS_KEY_RIGHT_SUPER        347
#define CHONPS_KEY_MENU               348

// Mouse Button Codes
#define CHONPS_MOUSE_BUTTON_1         0
#define CHONPS_MOUSE_BUTTON_2         1
#define CHONPS_MOUSE_BUTTON_3         2
#define CHONPS_MOUSE_BUTTON_4         3
#define CHONPS_MOUSE_BUTTON_5         4
#define CHONPS_MOUSE_BUTTON_6         5
#define CHONPS_MOUSE_BUTTON_7         6
#define CHONPS_MOUSE_BUTTON_8         7
#define CHONPS_MOUSE_BUTTON_LAST      CHONPS_MOUSE_BUTTON_8
#define CHONPS_MOUSE_BUTTON_LEFT      CHONPS_MOUSE_BUTTON_1
#define CHONPS_MOUSE_BUTTON_RIGHT     CHONPS_MOUSE_BUTTON_2
#define CHONPS_MOUSE_BUTTON_MIDDLE    CHONPS_MOUSE_BUTTON_3

#define BIT(x) (1 << x)

	// Core Log macros
#define CHONPS_CORE_TRACE(...) ::Chonps::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CHONPS_CORE_INFO(...) ::Chonps::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CHONPS_CORE_WARN(...) ::Chonps::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CHONPS_CORE_ERROR(...) ::Chonps::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CHONPS_CORE_CRITICAL(...) ::Chonps::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log macros
#define CHONPS_TRACE(...) ::Chonps::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CHONPS_INFO(...) ::Chonps::Log::GetClientLogger()->info(__VA_ARGS__)
#define CHONPS_WARN(...) ::Chonps::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CHONPS_ERROR(...) ::Chonps::Log::GetClientLogger()->error(__VA_ARGS__)
#define CHONPS_CRITICAL(...) ::Chonps::Log::GetClientLogger()->critical(__VA_ARGS__)

namespace Chonps
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

	// Vector Data Types
	typedef glm::vec1 vec1;
	typedef glm::vec2 vec2;
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;

	typedef glm::bvec1 vec1b;
	typedef glm::bvec2 vec2b;
	typedef glm::bvec3 vec3b;
	typedef glm::bvec4 vec4b;

	typedef glm::ivec1 vec1i;
	typedef glm::ivec2 vec2i;
	typedef glm::ivec3 vec3i;
	typedef glm::ivec4 vec4i;

	typedef glm::fvec1 vec1f;
	typedef glm::fvec2 vec2f;
	typedef glm::fvec3 vec3f;
	typedef glm::fvec4 vec4f;

	typedef glm::dvec1 vec1d;
	typedef glm::dvec2 vec2d;
	typedef glm::dvec3 vec3d;
	typedef glm::dvec4 vec4d;

	// Matrix Data Types
	typedef glm::mat2 mat2;
	typedef glm::mat3 mat3;
	typedef glm::mat4 mat4;
	typedef glm::mat3x2 mat3x2;
	typedef glm::mat2x3 mat2x3;
	typedef glm::mat2x4 mat2x4;
	typedef glm::mat4x2 mat4x2;

	typedef glm::dmat2 mat2d;
	typedef glm::dmat3 mat3d;
	typedef glm::dmat4 mat4d;
	typedef glm::dmat3x2 mat3x2d;
	typedef glm::dmat2x3 mat2x3d;
	typedef glm::dmat2x4 mat2x4d;
	typedef glm::dmat4x2 mat4x2d;

	typedef glm::fmat2 mat2f;
	typedef glm::fmat3 mat3f;
	typedef glm::fmat4 mat4f;
	typedef glm::fmat3x2 mat3x2f;
	typedef glm::fmat2x3 mat2x3f;
	typedef glm::fmat2x4 mat2x4f;
	typedef glm::fmat4x2 mat4x2f;

	// Vertex data type
	struct vertex
	{
		vec3 position;
		vec3 color;
		vec3 normal;
		vec2 texUV;
	};

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
		EventCategoryWindow = BIT(5)
	};

	// Events
	class Event
	{

		friend class EventDispatcher;

	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		bool Handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& eventType)
			: m_Event(eventType) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }

		static EventType GetStaticType() { return EventType::AppTick; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "AppTick"; }
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }

		static EventType GetStaticType() { return EventType::AppRender; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "AppRender"; }
	};

	class KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }
		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryKeyboard); }

	protected:
		KeyEvent(int keycode)
			: m_KeyCode(keycode) {}

		int m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, bool repeat)
			: KeyEvent(keycode), m_Repeat(repeat) {}

		inline bool IsRepeat() const { return m_Repeat; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (Repeat: " << m_Repeat << ")";
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyPressed; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "KeyPressed"; }

	private:
		bool m_Repeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyReleased; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "KeyReleased"; }
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyTyped; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "KeyTyped"; }
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float GetMouseX() const { return m_MouseX; }
		inline float GetMouseY() const { return m_MouseY; }

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryMouse); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseMoved; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "MouseMoved"; }

	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_OffsetX(xOffset), m_OffsetY(yOffset) {}

		inline float GetMouseScrollX() const { return m_OffsetX; }
		inline float GetMouseScrollY() const { return m_OffsetY; }

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_OffsetX << ", " << m_OffsetY;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseScrolled; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "MouseScrolled"; }

	private:
		float m_OffsetX, m_OffsetY;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }
		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton); }

	protected:
		MouseButtonEvent(int button)
			: m_Button(button) {}
		int m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseButtonPressed; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "MouseButtonPressed"; }
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseButtonReleased; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "MouseButtonReleased"; }
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		static EventType GetStaticType() { return EventType::WindowClose; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowClose"; }
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(int width, int height)
			: m_Width(width), m_Height(height) {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: (" << m_Width << ", " << m_Height << ")";
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WindowResize; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowResize"; }

	private:
		int m_Width, m_Height;

	};

	class WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		static EventType GetStaticType() { return EventType::WindowFocus; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowFocus"; }
	};

	class WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		static EventType GetStaticType() { return EventType::WindowLostFocus; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowLostFocus"; }
	};

	class WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(int x, int y)
			: m_WindowX(x), m_WindowY(y) {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: (" << m_WindowX << ", " << m_WindowY << ")";
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WindowMoved; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowMoved"; }

	private:
		int m_WindowX, m_WindowY;
	};

	// Window
	enum class WindowAPI
	{
		None = 0,
		Glfw,
		Win32
	};

	class Window
	{
	public:
		Window(std::string Title, int Width, int Height) {}

		using EventCallbackFn = std::function<void(Event&)>;

		virtual void OnUpdate() = 0;
		virtual void OnEvent(Event& e) = 0;
		virtual bool WindowIsOpen() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;


		// Window attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void LogEvents(bool enable) = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual void SetContextCurrent() = 0;

		virtual void Delete() = 0;
	};

	Window* createWindow(std::string title, unsigned int width, unsigned int height, bool fullScreen = false);

	bool setWindowAPI(WindowAPI api = WindowAPI::Glfw);
	bool windowTerminateAPI();

	WindowAPI getWindowContext();
	std::string getWindowContextName();


	void glfwInitWindowAPI();
	void glfwTerminateWindowAPI();

	// Input
	void windowInputInit();

	class Input
	{
	public:
		virtual bool KeyPressed(Window* window, int keycode) = 0;
		virtual bool KeyReleased(Window* window, int keycode) = 0;
		virtual bool MouseButtonPressed(Window* window, int button) = 0;
		virtual bool MouseButtonReleased(Window* window, int button) = 0;

		virtual vec2f GetMousePos(Window* window) = 0;
		virtual void GetMousePos(Window* window, float* xpos, float* ypos) = 0;
		virtual float GetMouseX(Window* window) = 0;
		virtual float GetMouseY(Window* window) = 0;

		virtual void SetMousePos(Window* window, float x, float y) = 0;

		virtual vec2i GetWindowPos(Window* window) = 0;
		virtual void GetWindowPos(Window* window, int* xpos, int* ypos) = 0;
		virtual vec2i GetWindowSize(Window* window) = 0;
		virtual void GetWindowSize(Window* window, int* width, int* height) = 0;

		virtual void SetMouseModeHide(Window* window, bool hide) = 0;

		virtual float GetTimestep() = 0;
	};

	// Input Functions
	bool keyPressed(Window* window, int keycode);
	bool keyReleased(Window* window, int keycode);
	bool mouseButtonPressed(Window* window, int button);
	bool mouseButtonReleased(Window* window, int button);

	vec2f getMousePos(Window* window);
	void getMousePos(Window* window, float* xpos, float* ypos);
	float getMouseX(Window* window);
	float getMouseY(Window* window);

	void setMousePos(Window* window, float x, float y);

	vec2i getWindowPos(Window* window);
	void getWindowPos(Window* window, int* xpos, int* ypos);
	vec2i getWindowSize(Window* window);
	void getWindowSize(Window* window, int* width, int* height);

	void setMouseModeHide(Window* window, bool hide);

	// Timestep Functions
	float getTimeSeconds();
	float getTimeMilliseconds();

	// Layer
	class Layer
	{
	public:
		Layer(std::string name = "Layer") {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}
		virtual bool LayerStatus() { return true; }
	};

	// LayerStack
	class LayerStack
	{
	public:
		void add(Layer* layer);
		void add_overlay(Layer* layer);

		void insert(Layer* layer, unsigned int index);
		void remove(Layer* layer);

		void delete_layer(Layer* layer);
		void delete_all();

		size_t size() { return Layers.size(); }
		std::vector<Layer*>::iterator begin() { return Layers.begin(); }
		std::vector<Layer*>::iterator end() { return Layers.end(); }

		// Use GetLayer function instead to avoid manipulating Layers data
		Layer* get(int index) { return Layers[index]; }

		std::vector<Layer*> Layers;

	private:
		unsigned int m_LayerInsertIndex = 0;
	};

	// Imgui Functions
	void ImguiInit(Window* window);

	void ImguiNewFrame();

	void ImguiNewFrame(Window* window);

	void ImguiRender();

	void ImguiShutdown();

	// ImguiEditor class
	class ImguiEditor : public Layer
	{
	public:
		ImguiEditor(Window* window);
		virtual ~ImguiEditor() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void NewFrame();
		void Render();

	private:
		Window* m_Window;
	};

	// Application
	class Application
	{
	public:
		Application(std::string Title, unsigned int width, unsigned int height, bool fullScreen = false);

		void Run();
		void OnEvent(Event& e);

		// Layers
		void add_layer(Layer* layer);
		void add_overlay(Layer* layer);
		void insert_layer(Layer* layer, unsigned int index);
		void remove_layer(Layer* layer);
		void delete_layer(Layer* layer);

		inline static Application& GetApp() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		inline float GetTimestep() { return m_Timestep; }
		inline void UpdateWindowRender(bool update) { m_UpdateWindowRender = update; }

	private:
		std::unique_ptr<Window> m_Window;
		LayerStack m_LayerStack;
		ImguiEditor* m_ImguiEditor;
		float m_Timestep = 0.0f;
		bool m_Running = true;
		bool m_UpdateWindowRender = true;

		bool OnWindowClose(WindowCloseEvent& e);


		static Application* s_Instance;
	};

	static unsigned int getAppWindowWidth()
	{
		return Application::GetApp().GetWindow().GetWidth();
	}

	static unsigned int getAppWindowHeight()
	{
		return Application::GetApp().GetWindow().GetHeight();
	}
}

#endif