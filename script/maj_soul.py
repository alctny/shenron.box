# /usr/bin/env python3
# -*- coding: utf-8 -*-

'''
雀魂相关内容获取爬虫

功能：
  1. 获取角色信息
  2. 获取角色表情包
  3. 获取角色语音包
  4. 获取角色立绘
  5. 获取角色背景
  6. 获取角色喜好礼物

爬取对象：
  - 雀魂 DB: https://mahjongsoul.club
  - 雀魂   : https://mjsinfo.pages.dev
'''

import requests
from bs4 import BeautifulSoup
import csv

MAHJONGSOUL_CLUB = "http://mahjongsoul.club"
CHARACTER_PAGE = "https://mahjongsoul.club/content/%E9%9B%80%E5%A3%AB?language=ja"
SELECTER = {
    "character": "#quicktabs-tabpage-qtcharactersgrid-0 > div > div.view-content > div > ul > li",
    "chinese": "div.link-wrapper > ul > li.zh-hant.active > a",
    "baseinfo": "#block-views-bonditem-block-14 > div > div > div > div > div",
}


def get_character_info(path: str):
  ''' 获取角色信息 '''
  try:
    response = requests.get(CHARACTER_PAGE, verify=False)
    if response.status_code != 200:
      raise Exception(f"获取雀士列表失败: {response.status_code}")
    page = response.text
    content = BeautifulSoup(page, "html.parser")
  except Exception as e:
    raise e

  title = ['名字', '顏色',    "假名"]
  data = [title]
  characters = content.select(SELECTER["character"])
  max = len(characters)
  counter = 0
  for character in characters:
    counter += 1
    # 获取角色详情页面信息
    char_info_page_url = character.select_one("a").get("href")
    char_info_page_url = f"{MAHJONGSOUL_CLUB}{char_info_page_url}"
    try:
      response = requests.get(char_info_page_url)
      if response.status_code != 200:
        print(f"获取 {char_info_page_url} 信息失败")
        continue
      # 繁体中文页面通过雀士列表跳转会有部分雀士出现 404 的情况，需要通过日语页面跳转
      href = BeautifulSoup(response.text, "html.parser").select_one(SELECTER["chinese"]).get("href")
      char_info_page_url = f"{MAHJONGSOUL_CLUB}{href}"
      response = requests.get(char_info_page_url)
      if response.status_code != 200:
        print(f"获取 {char_info_page_url} 信息失败")
        continue
      char_content = BeautifulSoup(response.text, "html.parser")
    except Exception as e:
      raise e

    # 开始获取角色基本信息
    char_info = {}
    for div in char_content.select(SELECTER["baseinfo"]):
      text = div.select_one("div").text
      if text == "":
        continue
      msg = text.split(":")
      if msg[0] in title:
        key, value = msg[0].strip(), msg[1].strip()
        char_info[key] = value
    data.append([char_info.get("名字", ""), char_info.get("顏色", ""), char_info.get("假名", "")])
    print(f"\r{counter:02}/{max} {char_info['名字']:40}\tOK", end="", flush=True)
  with open(path, "w", newline="") as f:
    csv.writer(f).writerows(data)


if __name__ == "__main__":
  get_character_info("character.csv")
