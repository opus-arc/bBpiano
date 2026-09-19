# bbpl — 余响（静态副本）

此目录以 [ChatGPT Sites 线上站点](https://bbpl-resonance.opus-arc.chatgpt.site) 的静态副本作为入口，并将原 MkDocs 研究文档合并到 GitHub Pages。原 ChatGPT 站点保留，未作修改。

运行 `python doc/site/build.py` 会把入口页面与 MkDocs 研究页面合成到仓库根目录的 `site/`，由 GitHub Pages 工作流发布。入口位于 `/bBpiano/`，研究位于 `/bBpiano/research/`。全部站内音频、图片、视频、样式和脚本都使用本目录中的资源；代码与许可证链接指向 GitHub。

- `audio/`：入口试听及研究音频比较共用的十段音频
- `assets/photos/`：人物照片
- `assets/identity/`：标识图片
- `css/`：页面样式
- `js/`：交互脚本
- `research/`：MkDocs 文稿及文稿引用的媒体资源
- `build.py`：组合构建脚本
