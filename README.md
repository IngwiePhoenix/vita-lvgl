# vita-lvgl
This is a very work-in-progress and early-in-conception state project to bring [lvgl](https://github.com/lvgl/lvgl) to the PlayStation Vita.

## Why?
Well, I have a project in my mind for which I need a GUI. Looking at the current options and available GUIs, I am not too satisfied with the situation. So, I will try to make my own implementation using LVGL. It is battletested on various platforms and oddly fits into the nieche of being an embeddable UI library that happens to be targeting embedded devices - which, in a way, applies to the PSVita. The main driver for this is that it's [input device API](https://docs.lvgl.io/v7/en/html/porting/indev.html) allows to configure specific buttons to do predefined actions. This means that after mapping the default inputs, screen and touch interfaces, this library *SHOULD* be ready to go and serve as a neat baseline for making GUIs for apps.

For now, my goal is to get the examples to run and compile. :)