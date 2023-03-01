#include "sfw/cfg/USE_SFML"
#include "sfw/GUI.hpp"

#include <SFML/Graphics.hpp>

#include <string> // to_string
#include <iostream> // cerr, for errors
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
    sfw::Theme::loadFont("demo/tahoma.ttf");
    sfw::Theme::loadTexture(defaultTheme.texturePath);
    sfw::Theme::textSize = 11;
    sfw::Theme::click.textColor      = hex2color("#191B18");
    sfw::Theme::click.textColorHover = hex2color("#191B18");
    sfw::Theme::click.textColorFocus = hex2color("#000");
    sfw::Theme::input.textColor = hex2color("#000");
    sfw::Theme::input.textColorHover = hex2color("#000");
    sfw::Theme::input.textColorFocus = hex2color("#000");
    sfw::Theme::input.textSelectionColor = hex2color("#8791AD");
    sfw::Theme::input.textPlaceholderColor = hex2color("#8791AD");
    sfw::Theme::PADDING = 2.f;
    sfw::Theme::windowBgColor = defaultTheme.backgroundColor;

    // A native SFML example Text object, to be manipulated via the GUI
    sf::Text text("Hello world!", sfw::Theme::getFont());
    text.setOrigin({text.getLocalBounds().width / 2, text.getLocalBounds().height / 2});
    text.setPosition({480, 240});

    // The main GUI controller:
    sfw::GUI demo(window);
    demo.setPosition(10, 10);

    // A horizontal layout to allow multiple subpanels side-by-side
    auto hbox = demo.add(new sfw::HBoxLayout());

    // A "form" panel (on the left)
    auto form = hbox->add(new sfw::FormLayout());

    // A text box to set the text of the SFML Text object (created above)
    form->addRow("Text",
        (new sfw::TextBox())->setPlaceholder("Type something!")->setText("Hello world!")
        ->setCallback([&](sfw::TextBox* tb) { text.setString(tb->getText());
            text.setOrigin({text.getLocalBounds().width / 2, text.getLocalBounds().height / 2});
        })
    );

    // Another text box (with text length limit & pulsating cursor)
    // Also keeping the widget pointer for use by other widgets later.
    auto textbox = new sfw::TextBox(50.f, sfw::TextBox::CursorStyle::PULSE);
    textbox->setText("Hello world!")->setMaxLength(5);
    form->addRow("Text with limit (5)", textbox);

    // Slider + progress bars for rotating the text
    auto sliderRotation = new sfw::Slider(1.f); // step = 1
    auto pbarRotation1 = new sfw::ProgressBar(200.f, sfw::Horizontal, sfw::LabelNone);
    auto pbarRotation2 = new sfw::ProgressBar(200.f, sfw::Horizontal, sfw::LabelOver);
    auto pbarRotation3 = new sfw::ProgressBar(200.f, sfw::Horizontal, sfw::LabelOutside);
    sliderRotation->setCallback([&](sfw::Slider* w) {
        text.setRotation(sf::degrees(w->getValue() * 360 / 100.f));
        pbarRotation1->setValue(w->getValue());
        pbarRotation2->setValue(w->getValue());
        pbarRotation3->setValue(w->getValue());
    });
    form->addRow("Rotation", sliderRotation);

    // Slider + progress bars for scaling the text
    auto sliderScale = new sfw::Slider();
    auto pbarScale1 = new sfw::ProgressBar(100, sfw::Vertical, sfw::LabelNone);
    auto pbarScale2 = new sfw::ProgressBar(100, sfw::Vertical, sfw::LabelOver);
    auto pbarScale3 = new sfw::ProgressBar(100, sfw::Vertical, sfw::LabelOutside);
    sliderScale->setCallback([&] (sfw::Slider* w) {
        float scale = 1 + w->getValue() * 2 / 100.f;
        text.setScale({scale, scale});
        pbarScale1->setValue(w->getValue());
        pbarScale2->setValue(w->getValue());
        pbarScale3->setValue(w->getValue());
    });
    form->addRow("Scale", sliderScale);

    // Options selector for color
    using OBColor = sfw::OptionsBox<sf::Color>;
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
            ->setCallback([&] (OBColor* w) { sfw::Theme::windowBgColor = w->getSelectedValue(); }));

    // Checbkoxes (to set text properties)

    form->addRow("Bold text", (new sfw::CheckBox())->setCallback([&] (sfw::CheckBox* w) {
        int style = text.getStyle();
        if (w->isChecked()) style |= sf::Text::Bold;
        else                style &= ~sf::Text::Bold;
        text.setStyle(style);
    }));

    form->addRow("Underlined text", (new sfw::CheckBox())->setCallback([&] (sfw::CheckBox* w) {
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
    auto layoutForVerticalProgressBars = new sfw::HBoxLayout();
    layoutForVerticalProgressBars->add(pbarScale1);
    layoutForVerticalProgressBars->add(pbarScale2);
    layoutForVerticalProgressBars->add(pbarScale3);
    form->addRow("Vertical progress bars", layoutForVerticalProgressBars);

    form->addRow("Default button", new sfw::Button("button"));

    // Custom bitmap button
    sf::Texture buttonimg;
    if (//!!Not here, as local temporary: sf::Texture buttonimg;
        !buttonimg.loadFromFile("demo/themed-button.png")) {
        cerr << "- Failed to load button theme image!\n";
    } else {
        form->addRow("Custom button", (new sfw::SpriteButton(buttonimg, "Play"))->setTextSize(20)
                                      ->setCallback([]/*(sfw::SpriteButton* w)*/ { /*compilation test*/ }));
    }

    // Another panel, on th right
    auto vboxRight = hbox->add(new sfw::VBoxLayout());

    // Theme selection
    vboxRight->add(new sfw::Label("Change theme:"));
    using OBTheme = sfw::OptionsBox<ThemeCfg>;
    vboxRight->add(new OBTheme())
        ->addItem("Windows 98", win98Theme)
        ->addItem("Default", defaultTheme)
        ->setCallback([&] (OBTheme* w) {
            auto& theme = w->getSelectedValue();
            sfw::Theme::loadTexture(theme.texturePath);
            sfw::Theme::windowBgColor = theme.backgroundColor;
        });

    // Textbox for new button labels
    auto hbox2 = vboxRight->add(new sfw::HBoxLayout());
    auto tbButtName = hbox2->add(new sfw::TextBox(100))->setText("Edit Me!")->setPlaceholder("Button label");
    hbox2->add(new sfw::Button("Create button", [&] { vboxRight->add(new sfw::Button(tbButtName->getText())); }));

    // Static images (also a cropped one)

    // Slider & progress bar for crop size
    sfw::Image* imgCrop = nullptr; // Hold your breath, see it created below!
    auto hbox3 = vboxRight->add(new sfw::HBoxLayout());
    hbox3->add(new sfw::Label("Crop square size:"));
    auto pbar = new sfw::ProgressBar(40);
    hbox3->add(pbar);
    // Slider for crop size
    hbox->add((new sfw::Slider(1.f, 100, sfw::Vertical))->setCallback([&](sfw::Slider* w) {
        pbar->setValue(w->getValue());
        // Show the slider value in a text box:
        textbox->setText(to_string((int)w->getValue()));
        imgCrop->setCropRect({{(int)(w->getValue() / 4), (int)(w->getValue() / 10)},
                              {(int)w->getValue(), (int)w->getValue()}});
    }));

    // Image directly from file
    auto vboximg = hbox->add(new sfw::VBoxLayout());
    vboximg->add(new sfw::Label("Image:"));
    vboximg->add(new sfw::Image("demo/image.png"));

    // Images from file, cropped
    vboximg->add(new sfw::Label("Image - crop:"));
    vboximg->add(new sfw::Image("demo/image.png", {{0, 33}, {24, 28}}));
    vboximg->add(new sfw::Label("Image - var. crop:"));
    vboximg->add(imgCrop = new sfw::Image("demo/sfml.png"));


    // Clear-background checkbox
    auto hbox4 = demo.add(new sfw::HBoxLayout());
    hbox4->add(new sfw::Label("Clear background"));
    hbox4->add((new sfw::CheckBox(true))->setCallback([&] (sfw::CheckBox* w) {
        clear_bgnd = w->isChecked();
    }));

    // Exit button
    demo.add(new sfw::Button("Quit", [&] { window.close(); }));


    // Start the event loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Send events to the demo GUI
            demo.process(event);

            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        // Clear screen
	if (clear_bgnd) window.clear(sfw::Theme::windowBgColor);

        // Render the GUI
        demo.render();

        // Draw the example SFML text
        window.draw(text);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
