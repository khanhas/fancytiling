# Overview
Fork of [PowerToys/Fancyzones](https://github.com/microsoft/PowerToys/tree/master/src/modules/fancyzones)

Most of Fancyzones features have been removed. In place, these features below are added and customised
### Features:
- Dynamic windows tiling: Single main window on the left and sub windows stacking on the right column. New window is auto added to stack.

- New keyboard shorcuts to interact with stacks and layout:
  - <kb>Win + Up/Down</kb>: Change window stacking position
  - <kb>Win + Left</kb>: Explicitly bring window to main zone
  - <kb>Win + Shift + Left/Right</kb>: Narrow/Broaden main zone width

**Demo:** https://i.imgur.com/g2x6zx6.mp4

- Shortcuts to interact with Virtual desktop:
  - <kb>Win + 1 to 9</kb>: Switch Virtual desktop
  - <kb>Win + Ctrl + 1 to 9</kb>: Move chosen window to virtual desktop

### How to install:
- Download [fancyzones.dll](https://github.com/khanhas/fancytiling/releases)
- Save it to `C:\Program Files\PowerToys\modules\FancyZones`. Since PowerToys has not supported custom module name so just paste and replace the original `fancyzones.dll`. Back it up if you want.
**Note:** Your path to PowerToys folder might be different
- Restart PowerToys

### Development
1. Clone
```bash
git clone https://github.com/microsoft/PowerToys
cd PowerToys/src/modules/
git clone https://github.com/khanhas/fancytiling
```

2. Open up PowerToys project in Visual Studio 2019. But first, check out requirements in PowerToys Github page.
3. In Solution Explorer panel, navigate to `modules` directory.
4. Create new directory under `modules` called `fancytiling`.
5. Right click at `fancytiling` -> Add -> Existing Project -> Choose `FancyZonesModule.vcxproj` and `FancyZonesLib.vcxproj` under `...\PowerToys\src\modules\fancytiling` folder (not original FancyZones folder).
6. Unload `FancyZones` project.
7. Now it's ready to build.