#include "sfw/GUI.hpp"

#include <SFML/Graphics.hpp>

#include <string> // to_string
#include <iostream> // cerr, for errors, cout for some "demo" info
#include <cassert>
using namespace std;

int main()
{
    //------------------------------------------------------------------------
    // Normal SFML app-specific setup....

    // Create the main window, as usual for any SFML app
    sf::RenderWindow window;
    window.create(sf::VideoMode({1024, 768}), "SFW Demo", sf::Style::Close|sf::Style::Resize);
    if (!window.isOpen()) {
        cerr << "- Failed to create the SFML window!\n";
        return EXIT_FAILURE;
    }
    window.setFramerateLimit(30);

    //------------------------------------------------------------------------
    // Setting up the GUI...

    // Just to spare some noise & typing...
    using sfw::Theme, sfw::hex2color, sfw::VBox, sfw::HBox, sfw::Form;

    // Customizing some theme props. (optional)

    Theme::DEFAULT.basePath = "demo/";
    Theme::DEFAULT.fontFile = "font/Vera.ttf";

    Theme::PADDING = 2.f;
    Theme::click.textColor      = hex2color("#191B18");
    Theme::click.textColorHover = hex2color("#191B18");
    Theme::click.textColorFocus = hex2color("#000");
    Theme::input.textColor = hex2color("#000");
    Theme::input.textColorHover = hex2color("#000");
    Theme::input.textColorFocus = hex2color("#000");
    Theme::input.textSelectionColor = hex2color("#8791AD");
    Theme::input.textPlaceholderColor = hex2color("#8791AD");

    // Some dynamically switcahble theme "quick config packs" to play with
    Theme::Cfg themes[] = {
        { "Default (\"Baseline\")", "demo/", "texture-sfw-baseline.png", hex2color("#e6e8e0"), 11, "font/Liberation/LiberationSans-Regular.ttf" },
        { "Classic ☺",              "demo/", "texture-sfw-classic.png",  hex2color("#e6e8e0"), 12, "font/Liberation/LiberationSans-Regular.ttf" },
        { "sfml-widgets's default", "demo/", "texture-sfmlwidgets-default.png", hex2color("#dddbde"), },
        { "sfml-widgets's Win98",   "demo/", "texture-sfmlwidgets-win98.png",   hex2color("#d4d0c8"), },
    };
    const size_t DEFAULT_THEME = 0;

    // The main GUI controller:
    sfw::GUI demo(window, themes[DEFAULT_THEME]);
    if (!demo) {
        return EXIT_FAILURE; // Errors have already been printed to cerr.
    }
    demo.setPosition(10, 10);

    // A native SFML example Text object, to be manipulated via the GUI
    sf::Text text("Hello world!", Theme::getFont());
    text.setOrigin({text.getLocalBounds().width / 2, text.getLocalBounds().height / 2});
    text.setPosition({480, 300});

    // A horizontal layout for multiple panels side-by-side
    auto main_hbox = demo.add(new HBox);

    // A "form" panel on the left
    auto form = main_hbox->add(new Form);

    // A text box to set the text of the example SFML object (created above)
    form->add("Text", (new sfw::TextBox)
        ->setPlaceholder("Type something!")
        ->setText("Hello world!")
        ->setCallback([&](auto* w) {
            text.setString(w->getText());
            text.setOrigin({text.getLocalBounds().width / 2, text.getLocalBounds().height / 2});
        })
    );

    // Another text box, with length limit & pulsating cursor
    // Creating it without a "junk" local variable now:
    form->add("Text with limit (5)",
                 (new sfw::TextBox(50.f, sfw::TextBox::CursorStyle::PULSE))
                     ->setMaxLength(5)
                     ->setText("Hello world!")
    );
    // Note: the label above ("Text with limit (5)") will also become the name
    // of the added widget (the TextBox), which can then be used for retrieving
    // it later. (See it used by the crop slider somewhere below.)
    // It can also be overridden (e.g. to avoid duplicates), like:
    // form->add("repeated label or overly long text", widget, "widgetname");
    // (Widget names are optional.)

    // Slider + progress bars for rotating the text
    auto sliderForRotation = new sfw::Slider(1); // granularity = 1%
        auto pbarRotation1 = new sfw::ProgressBar(200.f, sfw::Horizontal, sfw::LabelNone);
        auto pbarRotation2 = new sfw::ProgressBar(200.f, sfw::Horizontal, sfw::LabelOver);
        auto pbarRotation3 = new sfw::ProgressBar(200.f, sfw::Horizontal, sfw::LabelOutside);
    sliderForRotation->setCallback([&](auto* w) {
        text.setRotation(sf::degrees(w->getValue() * 360 / 100.f));
        pbarRotation1->setValue(w->getValue());
        pbarRotation2->setValue(w->getValue());
        pbarRotation3->setValue(w->getValue());
    });
    form->add("Rotation", sliderForRotation);

    // Slider + progress bars for scaling the text
    auto pbarScale1 = new sfw::ProgressBar(100, sfw::Vertical, sfw::LabelNone);
    auto pbarScale2 = new sfw::ProgressBar(100, sfw::Vertical, sfw::LabelOver);
    auto pbarScale3 = new sfw::ProgressBar(100, sfw::Vertical, sfw::LabelOutside);
    // (Using the denser, "functional" code style again)
    form->add("Scale", (new sfw::Slider())->setCallback([&](auto* w) {
        float scale = 1 + w->getValue() * 2 / 100.f;
        text.setScale({scale, scale});
        pbarScale1->setValue(w->getValue());
        pbarScale2->setValue(w->getValue());
        pbarScale3->setValue(w->getValue());
    }));

    // Text color selector
    using OBColor = sfw::OptionsBox<sf::Color>;
    // (Keeping in a var only to clone it later.)
    auto optTxtColor = (new OBColor([&](auto* w) {
            text.setFillColor(w->getSelectedValue());
            w->setColor(w->getSelectedValue());
        }))
        ->addItem("Red", sf::Color::Red)
        ->addItem("Blue", sf::Color::Blue)
        ->addItem("Green", sf::Color::Green)
        ->addItem("Yellow", sf::Color::Yellow)
        ->addItem("White", sf::Color::White);

    // Cloning it for selecting the window bg. color, too...
    //!NOTE: MUST BE COPIED BEFORE ADDING IT (optTxtColor) TO A CONTAINER!
    auto optClone = (new OBColor(*optTxtColor))
        ->addItem("Default", themes[DEFAULT_THEME].bgColor)
        ->setCallback([&](auto* w) { Theme::windowBgColor = w->getSelectedValue(); });

    form->add("Text color", optTxtColor);
    form->add("Screen bgnd. (copied widget)", optClone);

    // Checkboxes to set text properties
    // (Note the use of the "new-less" (copying) add() style, for a change.)
    form->add("Underlined text", sfw::CheckBox([&](auto* w) {
        int style = text.getStyle();
        if (w->isChecked()) style |=  sf::Text::Underlined;
        else                style &= ~sf::Text::Underlined;
        text.setStyle(style);
    }));
    form->add("Bold text", sfw::CheckBox([&](auto* w) {
        int style = text.getStyle();
        if (w->isChecked()) style |=  sf::Text::Bold;
        else                style &= ~sf::Text::Bold;
        text.setStyle(style);
    }));

    // Attach the horiz. progress bars (used for text rotation) to the form
    form->add("Progress bar (label = None)",    pbarRotation1);
    form->add("Progress bar (label = Over)",    pbarRotation2);
    form->add("Progress bar (label = Outside)", pbarRotation3);

    // Attach the vert. progress bars (used for text scaling) to a new box
    auto layoutForVerticalProgressBars = new HBox;
    //!! Issue #109: Document that manipulating unowned, free-standing layouts is UB!
    form->add("Vertical progress bars", layoutForVerticalProgressBars);
    layoutForVerticalProgressBars->add(pbarScale1);
    layoutForVerticalProgressBars->add(pbarScale2);
    layoutForVerticalProgressBars->add(pbarScale3);

    form->add("Default button", sfw::Button("button"));
    cout << "Button text retrieved via name lookup: \"" << ((sfw::Button*)demo.get("Default button"))->getText() << "\"\n";

    auto utf8button_tag = "UTF-8"; // Will also use it to recall the button!
    form->add(utf8button_tag, sfw::Button("Ψ ≠ 99° ± β")); // Note: this source is already UTF-8 encoded.
    cout << "UTF-8 button text got back as: \"" << ((sfw::Button*)demo.get(utf8button_tag))->getText() << "\"\n";

    // Custom bitmap button
    sf::Texture buttonimg; //! DON'T put this inside the if () as local temporary (as I once have... ;) )
    if (buttonimg.loadFromFile("demo/sfmlwidgets-themed-button.png")) // SFML would print an error if failed
    {
        form->add("Custom button", (new sfw::ImageButton(buttonimg, "Play"))->setTextSize(20)
                                      ->setCallback([]/*(sfw::ImageButton* w)*/ { /*compilation test*/ }));
    }

    // A panel in the middle
    auto middle_panel = main_hbox->add(new VBox);

    // Theme selection
    using OBTheme = sfw::OptionsBox<Theme::Cfg>;
    middle_panel->add(sfw::Label("Change theme:"));
    auto themeselect = new OBTheme([&](auto* w) {
        const auto& themecfg = w->getSelectedValue();
        demo.setTheme(themecfg); // Swallowing the error for YOLO reasons ;)
    });
    for (auto& t: themes) { themeselect->addItem(t.name, t); }
    themeselect->selectItem(DEFAULT_THEME);
    middle_panel->add(themeselect, "theme-selector");

    // Theme font size slider
    // (Changes the font size directly of the cfg. data stored in "theme-selector",
    // so it will remember the new size(s)!)
    middle_panel->add(sfw::Label("Theme font size (use the m. wheel):"));
    middle_panel->add(sfw::Slider(10, 100))
        ->setValue(30)
        ->setCallback([&] (auto* w){
            assert(w->get("theme-selector"));
            auto& themecfg = ((OBTheme*)(w->get("theme-selector")))->getSelectedValueRef();
            themecfg.textSize = 8 + size_t(w->getValue() / 10);
            demo.setTheme(themecfg);
        });


    // Image views...

    // Image directly from file
    auto vboximg = main_hbox->add(new VBox);
    vboximg->add(sfw::Label("Image (from file):"));
    vboximg->add(sfw::Image("demo/some image.png"));

    // Image crop from file
    vboximg->add(sfw::Label("Image crop:"));
    vboximg->add(sfw::Image("demo/some image.png", {{0, 33}, {24, 28}}));
    vboximg->add(sfw::Label("Image crop varied:"));

    // Image from file, cropped dynamically
    sfw::Image* imgCrop = new sfw::Image("demo/martinet-dragonfly.jpg");
    vboximg->add(imgCrop);
    vboximg->add(sfw::Label("(Art: © Édouard Martinet)"))->setStyle(sf::Text::Style::Italic);

    auto right_bar = main_hbox->add(new VBox);

    // Show the GUI textures, for some insight/diagnostics
    right_bar->add(sfw::Label("Theme textures:"));
    auto txbox = right_bar->add(new HBox);
    auto tximg = (new sfw::Image(Theme::getTexture())) // note: e.g. the ARROW is at {{0, 42}, {6, 6}}
        ->scale(2);
    txbox->add(sfw::Slider(1.f, 100.f, sfw::Vertical))
        ->setCallback([&](auto* w) { tximg->scale(1 + (100.f - w->getValue()) / 25.f); })
        ->setStep(25.f)
        ->setValue(75.f);
    txbox->add(tximg);

    // Textbox for the labels of new buttons created by the button-factory button...
    auto boxfactory = middle_panel->add(new HBox);
    auto labeller = boxfactory->add(sfw::TextBox(100))->setText("Edit Me!")->setPlaceholder("Button label");
    boxfactory->add(sfw::Button("Create button", [&] {
        middle_panel->addAfter(boxfactory, new sfw::Button(labeller->getText()));
    }));

    // Slider & progress bar for cropping an Image widget
    auto boxcrop = middle_panel->add(new HBox);
        boxcrop->add(sfw::Label("Crop square size:"));
        boxcrop->add(sfw::ProgressBar(40), "cropbar");
    middle_panel
        ->add(sfw::Slider(1, 100))
            ->setCallback([&](auto* w) {
                ((sfw::ProgressBar*)w->get("cropbar"))->setValue(w->getValue());
                // Show the slider value in a text box retrieved by its name:
                auto tbox = (sfw::TextBox*)w->get("Text with limit (5)");
                if (!tbox) cerr << "Named TextBox not found! :-o\n";
                else tbox->setText(to_string((int)w->getValue()));
                imgCrop->setCropRect({{(int)(w->getValue() / 4), (int)(w->getValue() / 10)},
                                  {(int)(w->getValue() * 1.4), (int)w->getValue()}});
            });

    // Clear-background checkbox
    auto hbox4 = demo.add(new HBox);
    hbox4->add(sfw::Label("Clear background"));
    hbox4->add(sfw::CheckBox([&](auto* w) { Theme::clearWindow = w->isChecked(); }, true));

    //form->add("This is just some text on its own."); // Wouldn't compile yet...

    // Exit button
    demo.add(sfw::Button("Quit", [&] { window.close(); }));


    // Start the event loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Send events to the demo GUI
            demo.process(event);

            if (event.type == sf::Event::Closed ||
                event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        // Render the GUI
        demo.render();

        // Draw the example SFML text
        window.draw(text);

        // Show the updated window
        window.display();
    }

    return EXIT_SUCCESS;
}
