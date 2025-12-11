#include "engine/ui/ui_document.hpp"

#include "engine/ui/ui_types.hpp"

#include <algorithm>
#include <sstream>
#include <string>

namespace engine::ui {

namespace {

class DocumentBuilder {
public:
    DocumentBuilder() = default;

    auto build(UiElement root,
               std::span<const std::string_view> stylesheets,
               std::string_view template_markup) -> UiDocument {
        std::string body_markup = render_element(std::move(root));

        std::string body_template(template_markup);
        if (body_template.empty()) {
            body_template = std::string{kScreenContentToken};
        }

        const auto token_pos = body_template.find(kScreenContentToken);
        if (token_pos != std::string::npos) {
            body_template.replace(token_pos, kScreenContentToken.size(), body_markup);
        } else {
            body_template.append(body_markup);
        }

        if (body_template.find("<body") == std::string::npos) {
            body_template = "<body>" + body_template + "</body>";
        }

        std::string style_markup;
        for (const auto sheet : stylesheets) {
            if (sheet.empty()) {
                continue;
            }
            style_markup += "<style type=\"text/rcss\">\n";
            style_markup.append(sheet.data(), sheet.size());
            style_markup += "\n</style>";
        }

        std::string document;
        document.reserve(style_markup.size() + body_template.size() + 32);
        document += "<rml><head>";
        document += style_markup;
        document += "</head>";
        document += body_template;
        document += "</rml>";

        return UiDocument{
            .markup = std::move(document),
            .events = std::move(events_)
        };
    }

private:
    auto render_element(UiElement element) -> std::string {
        if (element.tag.empty()) {
            return {};
        }

        if (element.id.empty() && !element.events.empty()) {
            element.id = next_auto_id();
        }

        std::ostringstream oss;
        oss << "<" << element.tag;

        if (!element.id.empty()) {
            oss << " id=\"" << element.id << "\"";
        }

        if (!element.classes.empty()) {
            oss << " class=\"";
            for (std::size_t i = 0; i < element.classes.size(); ++i) {
                oss << element.classes[i];
                if (i + 1 < element.classes.size()) {
                    oss << " ";
                }
            }
            oss << "\"";
        }

        for (const auto& [key, value] : element.attributes) {
            oss << " " << key << "=\"" << escape(value) << "\"";
        }

        oss << ">";

        if (element.text.has_value()) {
            oss << escape(*element.text);
        }

        for (auto& child : element.children) {
            oss << render_element(std::move(child));
        }

        oss << "</" << element.tag << ">";

        for (const auto& event : element.events) {
            events_.push_back(UiEventBinding{
                .id = element.id,
                .type = event.type,
                .handler = event.handler
            });
        }

        return oss.str();
    }

    static auto escape(std::string_view text) -> std::string {
        std::string result;
        result.reserve(text.size());
        for (const char ch : text) {
            switch (ch) {
                case '&':
                    result.append("&amp;");
                    break;
                case '<':
                    result.append("&lt;");
                    break;
                case '>':
                    result.append("&gt;");
                    break;
                case '"':
                    result.append("&quot;");
                    break;
                default:
                    result.push_back(ch);
                    break;
            }
        }
        return result;
    }

    [[nodiscard]] auto next_auto_id() -> std::string {
        return "ui_auto_" + std::to_string(auto_counter_++);
    }

    std::vector<UiEventBinding> events_{};
    int auto_counter_{0};
};

}  // namespace

auto build_ui_document(UiElement root,
                       std::span<const std::string_view> stylesheets,
                       std::string_view template_markup) -> UiDocument {
    DocumentBuilder builder{};
    return builder.build(std::move(root), stylesheets, template_markup);
}

}  // namespace engine::ui

