#ifndef SOURCE_NOTIFICATIONS_HPP
#define SOURCE_NOTIFICATIONS_HPP

#include <includes_pch.h>
// A little playground with notifications
// Need to be done in future with icons, custom fonts, animations, blur and blackjack
// Ideas:
// Each notification is a new window and viewport
// But as it's notification it shouldn't be shown in the taskbar (WS_EX_TOOLWINDOW in windows)
// I ran out of ideas for now, i want to sleep :)

int GetCurrentMonitor()
{
	auto cur_window = ImGui::GetCurrentWindow();
	auto monitor = cur_window->Viewport->PlatformMonitor;
	return monitor;
}

ImVec2 GetMonitorResolution(int idMonitor = GetCurrentMonitor())
{
	ImGuiContext& g = *GImGui;
	auto monitor = g.PlatformIO.Monitors[idMonitor];
	return monitor.WorkSize;
}

typedef struct {
	uint64_t id;
	std::string label;
	std::string text;
	std::string icon;
	ImVec4 color_icon;
	std::function<void()> callback;
	std::chrono::steady_clock::time_point time_point;
} NotifyData;

class NotificationSystem {
	int max_duration = 5000; // 5 seconds
	int nOnScreenMax = 3;

	std::vector<NotifyData> notifications;
	std::vector<uint64_t> to_delete;
	ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | 
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;
	int local_counter;
public:
	NotificationSystem()
	{
		int reserve_size = 32;
		notifications.reserve(reserve_size);
		to_delete.reserve(reserve_size);
	}
	~NotificationSystem()
	{
		notifications.clear();
		to_delete.clear();
		local_counter = 0;
	}

	void AddNotification(std::string_view label, std::string_view text)
	{
		// std::chrono::steady_clock::time_point 
		auto time_point = std::chrono::steady_clock::now();
		NotifyData data = { local_counter++, label.data(), text.data(), "", {}, nullptr, time_point };
		notifications.emplace_back(std::move(data));
	}

	void AddNotificationIcon(std::string_view label, std::string_view text, std::string_view icon, ImVec4 icon_color = ImVec4(30, 144, 255, 255))
	{
		auto time_point = std::chrono::steady_clock::now();
		NotifyData data = { local_counter++, label.data(), text.data(), icon.data(), icon_color, nullptr, time_point };
		notifications.emplace_back(std::move(data));
	}

	void AddNotificationCustomElements(std::string_view label, std::string_view icon, ImVec4 icon_color, std::function<void()> callback)
	{
		auto time_point = std::chrono::steady_clock::now();
		NotifyData data = { local_counter++, label.data(), "", icon.data(), icon_color, callback, time_point};
		notifications.emplace_back(std::move(data));
	}

	void RenderNotifications()
	{
		int cur_notifaction = 0;
		for (auto&& notification : notifications)
		{
			auto& [id, label, text, icon, icon_color, callback, time_point] = notification;

			auto cur_time_point = std::chrono::steady_clock::now();
			auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time_point - time_point).count();

			if (timeElapsed >= max_duration)
				to_delete.push_back(id);

			if (cur_notifaction >= nOnScreenMax)
			{
				time_point = cur_time_point;
				continue;
			}
			
			ImVec2 szNotification = { 300.f, 100.f };
			ImVec2 szNotifyPadding = { 10.f, 5.f };
			ImVec2 szMonitor = GetMonitorResolution(0);
			auto posWindow = szMonitor - szNotification - szNotifyPadding;
			posWindow.y -= (szNotifyPadding.y + szNotification.y) * cur_notifaction;

			ImGui::SetNextWindowSize(szNotification, ImGuiCond_Appearing);
			ImGui::SetNextWindowPos(posWindow, ImGuiCond_Appearing);

			ImGui::Begin(label.data(), 0, flags);

			if (!icon.empty())
			{
				auto szText = ImGui::CalcTextSize(icon.data());
				auto posCur = ImGui::GetCursorScreenPos();
				ImGui::TextColored(icon_color, icon.data());
				ImGui::SameLine();
				posCur.x += szText.x + 20.f;
				ImGui::SetCursorScreenPos(posCur);
				ImGui::Text(label.data());
			}
			else
				ImGui::Text(label.data());
			ImGui::Separator();

			if (callback)
			{
				callback();
			}
			else
				ImGui::Text(text.data());

			ImGui::End();
			cur_notifaction++;
		}

		for (auto&& id : to_delete)
		{
			for (int i = 0; auto && notification : notifications)
			{
				if (id == notification.id)
				{
					notifications.erase(notifications.begin() + i);
					break;
				}
				++i;
			}
		}

	}
};

#endif // !SOURCE_NOTIFICATIONS_HPP