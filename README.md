# FancyTiling
Fork of [PowerToys/Fancyzones](https://github.com/microsoft/PowerToys/tree/master/src/modules/fancyzones)  

Organising workplace and boost workflow with fast, optimised and dynamic windows tiling manager. For Windows users that want a taste of glorious Linux desktop experience.  
Most of Fancyzones features have been removed. In place, these features below are added and customised. Feel free to request or pull request new features.

### Features:
- Dynamic windows tiling: Single main window on the left and sub windows stacking on the right column. New window is auto added to stack.

- New keyboard shorcuts to interact with stacks and layout:
  - <kbd>Win</kbd> <kbd>Up/Down</kbd>: Change window stacking position
  - <kbd>Win</kbd> <kbd>Left</kbd>: Explicitly bring window to main zone
  - <kbd>Win</kbd> <kbd>Shift</kbd> <kbd>Left/Right</kbd>: Narrow/Broaden main zone width

**Demo:** https://i.imgur.com/g2x6zx6.mp4

- Shortcuts to interact with Virtual desktops:
  - <kbd>Win</kbd> <kbd>1 to 9</kbd>: Switch Virtual desktop
  - <kbd>Win</kbd> <kbd>Ctrl</kbd> <kbd>1 to 9</kbd>: Move chosen window to virtual desktop

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