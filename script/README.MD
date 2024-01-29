# 自用脚本列表

一些自己使用的脚本，包括 shell 脚本，Python 爬虫等

## 地理编码爬取（adcode.py）

*nbs_adcode.py* 用于从[关于更新全国统计用区划代码和城乡划分代码的公告](https://www.stats.gov.cn/sj/tjbz/tjyqhdmhcxhfdm/2022/index.html)获取最新的城乡划分代码并保存为 csv 文件。

- 使用：

```bash
python nbs_adcode.py [url] [sigle]
```

- 参数说明
  - -u/--url: 起始页面，你可以选择只获取单一省，市，县...留空则表示从根页面开始（爬取所有省）
  - -s/--sigle：是否保存为单个文件，如果为 True 结果会保存在 **adcode.csv** 文件，如果为 False 结果会保存在多个 csv 文件

## 雀魂爬虫脚本（maj_soul.py）

*maj_color.py* 可用爬取[雀魂](https://game.maj-soul.com/1/)角色对应颜色编码，资讯由[雀魂 DB](https://mahjongsoul.club)提供


## maj_soul.py

爬取内容包括（施工当中），数据由[雀魂攻略](https://mjsinfo.pages.dev) 提供：

- 雀姬基本信息
- 雀姬表情图
- 雀姬服饰图
- 雀姬喜好礼物
- 雀姬语音（文字和音频）
