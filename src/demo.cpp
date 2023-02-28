#include "sfw/Gui.hpp"
#include "sfw/util/hex2color.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

struct ThemeCfg
{
    sf::Color backgroundColor;
    std::string texturePath;
};

//----------------------------------------------------------------------------
int main()
{
    ThemeCfg defaultTheme = {
        hex2color("#e6e8e0"),
        "demo/texture-default.png"
    };

    ThemeCfg win98Theme = {
        hex2color("#d4d0c8"),
        "demo/texture-win98.png"
    };

    bool clear_bgnd = true;

    // Create the main window
    sf::RenderWindow window;
    window.create(sf::VideoMode({1024, 768}), "SFW Demo", sf::Style::Close|sf::Style::Resize);
    if (!window.isOpen()) {
        cerr << "- Failed to create SFML window!\n";
        return EXIT_FAILURE;
    }
    window.setFramerateLimit(30);

    // Start building the GUI...

    // Customize the config
    gui::Theme::loadFont("demo/tahoma.ttf");
    gui::Theme::loadTexture(defaultTheme.texturePath);
    gui::Theme::textSize = 11;
    gui::Theme::click.textColor      = hex2color("#191B18");
    gui::Theme::click.textColorHover = hex2color("#191B18");
    gui::Theme::click.textColorFocus = hex2color("#000");
    gui::Theme::input.textColor = hex2color("#000");
    gui::Theme::input.textColorHover = hex2color("#000");
    gui::Theme::input.textColorFocus = hex2color("#000");
    gui::Theme::input.textSelectionColor = hex2color("#8791AD");
    gui::Theme::input.textPlaceholderColor = hex2color("#8791AD");
    gui::Theme::PADDING = 2.f;
    gui::Theme::windowBgColor = defaultTheme.backgroundColor;

    // A native SFML example Text object, to be manipulated via the GUI
    sf::Text text("Hello world!", gui::Theme::getFont());
    text.setOrigin({text.getLocalBounds().width / 2, text.getLocalBounds().height / 2});
    text.setPosition({480, 240});

    // The main GUI controller:
    gui::Main demo(window);
    demo.setPosition(10, 10);

    // A horizontal layout to allow multiple subpanels side-by-side
    auto hbox = demo.addHBoxLayout();

    // A "form" panel (on the left)
    auto form = hbox->addFormLayout();

    // A text box to set the text of the SFML Text object (created above)
    form->addRow("Text",
        (new gui::TextBox())->setPlaceholder("Type something!")->setText("Hello world!")
        ->setCallback([&](gui::TextBox* tb) { text.setString(tb->getText());
            text.setOrigin({text.getLocalBounds().width / 2, text.getLocalBounds().height / 2});
        })
    );

    // Another text box (with text length limit & pulsating cursor)
    form->addRow("Text with limit (5)",
        (new gui::TextBox(200.f, gui::TextBox::CursorStyle::PULSE))->setText("Hello world!")->setMaxLength(5));

    // Slider + progress bars for rotating the text
    auto sliderRotation = new gui::Slider(1.f); // step = 1
    auto pbarRotation1 = new gui::ProgressBar(200.f, gui::Horizontal, gui::LabelNone);
    auto pbarRotation2 = new gui::ProgressBar(200.f, gui::Horizontal, gui::LabelOver);
    auto pbarRotation3 = new gui::ProgressBar(200.f, gui::Horizontal, gui::LabelOutside);
    sliderRotation->setCallback([&](gui::Slider* w) {
        text.setRotation(sf::degrees(w->getValue() * 360 / 100.f));
        pbarRotation1->setValue(w->getValue());
        pbarRotation2->setValue(w->getValue());
        pbarRotation3->setValue(w->getValue());
    });
    form->addRow("Rotation", sliderRotation);

    // Slider + progress bars for scaling the text
    auto sliderScale = new gui::Slider();
    auto pbarScale1 = new gui::ProgressBar(100, gui::Vertical, gui::LabelNone);
    auto pbarScale2 = new gui::ProgressBar(100, gui::Vertical, gui::LabelOver);
    auto pbarScale3 = new gui::ProgressBar(100, gui::Vertical, gui::LabelOutside);
    sliderScale->setCallback([&] (gui::Slider* w) {
        float scale = 1 + w->getValue() * 2 / 100.f;
        text.setScale({scale, scale});
        pbarScale1->setValue(w->getValue());
        pbarScale2->setValue(w->getValue());
        pbarScale3->setValue(w->getValue());
    });
    form->addRow("Scale", sliderScale);

    // Options selector for color
    using OBColor = gui::OptionsBox<sf::Color>;
    auto opt = (new OBColor()) // <- Keeping in a var now, just to clone it later...
        ->addItem("Red", sf::Color::Red)
        ->addItem("Blue", sf::Color::Blue)
        ->addItem("Green", sf::Color::Green)
        ->addItem("Yellow", sf::Color::Yellow)
        ->addItem("White", sf::Color::White)
        ->setCallback([&] (OBColor* w) { text.setFillColor(w->getSelectedValue()); });
    form->addRow("Text color", opt);

    // A cloned options selector (for backgroud color)
    form->addRow("Bgnd. (via cloned OptionsBox)", (new OBColor(*static_cast<OBColor*>(opt)))
            ->setCallback([&] (OBColor* w) { gui::Theme::windowBgColor = w->getSelectedValue(); }));

    // Checbkoxes (to set text properties)

    form->addRow("Bold text", (new gui::CheckBox())->setCallback([&] (gui::CheckBox* w) {
        int style = text.getStyle();
        if (w->isChecked()) style |= sf::Text::Bold;
        else                style &= ~sf::Text::Bold;
        text.setStyle(style);
    }));

    form->addRow("Underlined text", (new gui::CheckBox())->setCallback([&] (gui::CheckBox* w) {
        int style = text.getStyle();
        if (w->isChecked()) style |= sf::Text::Underlined;
        else                style &= ~sf::Text::Underlined;
        text.setStyle(style);
    }));

    // Attach the horiz. progress bars (used for text rotation) to a layout
    form->addRow("Progress bar (label = None)", pbarRotation1);
    form->addRow("Progress bar (label = Over)", pbarRotation2);
    form->addRow("Progress bar (label = Outside)", pbarRotation3);

    // Setup a stacked layout for the vertical progress bars (used for text scaling)
    auto layoutForVerticalProgressBars = new gui::HBoxLayout();
    layoutForVerticalProgressBars->add(pbarScale1);
    layoutForVerticalProgressBars->add(pbarScale2);
    layoutForVerticalProgressBars->add(pbarScale3);
    form->addRow("Vertical progress bars", layoutForVerticalProgressBars);

    form->addRow("Default button", new gui::Button("button"));

    // Custom bitmap button
    sf::Texture buttonimg;
    if (//!!Not here, as local temporary: sf::Texture buttonimg;
        !buttonimg.loadFromFile("demo/themed-button.png")) {
        cerr << "- Failed to load button theme image!\n";
    } else {
        form->addRow("Custom button", (new gui::SpriteButton(buttonimg, "Play"))->setTextSize(20)
                                      ->setCallback([]/*(gui::SpriteButton* w)*/ { /*compilation test*/ }));
    }

    // Another panel, on th right
    auto vboxRight = hbox->addVBoxLayout();

    // Theme selection
    vboxRight->addLabel("Change theme:");
    using OBTheme = gui::OptionsBox<ThemeCfg>;
    vboxRight->add((new OBTheme())
        ->addItem("Windows 98", win98Theme)
        ->addItem("Default", defaultTheme)
        ->setCallback([&] (OBTheme* w) {
            auto& theme = w->getSelectedValue();
            gui::Theme::loadTexture(theme.texturePath);
            gui::Theme::windowBgColor = theme.backgroundColor;
    }));

    // Textbox for new button labels
    auto hbox2 = vboxRight->addHBoxLayout();
    auto tbbutt = (new gui::TextBox(100))->setText("Edit Me!")->setPlaceholder("Button label");
    hbox2->add(tbbutt);
    hbox2->addButton("Create button", [&] { vboxRight->add(new gui::Button(tbbutt->getText())); });

    // Static images (also a cropped one)

    // Progress bar for crop size
    gui::Image* imgCrop = nullptr; // Hold your breath, see it created below!
    auto hbox3 = vboxRight->addHBoxLayout();
    hbox3->addLabel("Crop square size:");
    auto pbar = new gui::ProgressBar(40);
    hbox3->add(pbar);
    // Slider for crop size
    hbox->add((new gui::Slider(1.f, 100, gui::Vertical))->setCallback([&] (gui::Slider* w) {
        cerr << "Slider value: " << w->getValue() << endl;
        pbar->setValue(w->getValue());
        imgCrop->setCropRect({{(int)(w->getValue() / 4), (int)(w->getValue() / 10)},
                              {(int)w->getValue(), (int)w->getValue()}});
    }));

    // Image directly from file
    auto vboximg = hbox->addVBoxLayout();
    vboximg->add(new gui::Label("Image:"));
    vboximg->add(new gui::Image("demo/image.png"));

    // Images from file, cropped
    vboximg->add(new gui::Label("Image - crop:"));
    vboximg->add(new gui::Image("demo/image.png", {{0, 33}, {24, 28}}));
    vboximg->add(new gui::Label("Image - var. crop:"));
    vboximg->add(imgCrop = new gui::Image("demo/sfml.png"));


    // Clear-background checkbox
    auto hbox4 = demo.addHBoxLayout();
    hbox4->add(new gui::Label("Clear background"));
    hbox4->add((new gui::CheckBox(true))->setCallback([&] (gui::CheckBox* w) {
        clear_bgnd = w->isChecked();
    }));

    // Exit button
    demo.addButton("Quit", [&] { window.close(); });


    // Start the event loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Send events to demo
            demo.onEvent(event);
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        // Clear screen
	if (clear_bgnd) window.clear(gui::Theme::windowBgColor);
        window.draw(demo);
        window.draw(text);
        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
