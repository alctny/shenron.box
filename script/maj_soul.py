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

- 雀魂   : https://mjsinfo.pages.dev
'''

import requests
from bs4 import BeautifulSoup
from typing import Dict
import os
import csv
import json

BASE_URL = "https://mjsinfo.pages.dev"
CHARACTER_PAGE = "https://mjsinfo.pages.dev/character"
BASE_DIR = "maj_soul"
RES_JSON = "res.json"


def new_save_dir(name: str) -> str:
  """ 新建保存目录 """
  path = os.path.join(BASE_DIR, name)
  if not os.path.exists(path):
    os.makedirs(path)
  return path


def new_beautiful_soup(url: str, trymax: int = 3) -> BeautifulSoup:
  for count in range(trymax):
    try:
      r = requests.get(url)
      return BeautifulSoup(r.text, 'html.parser')
    except Exception as e:
      # 执行到此处的时候第一次请求已经结束，而且 range 会从 0 开始遍历，所以这里 count + 2
      print(f"retry...{count+2}")
      if count == trymax - 1:
        raise e


def get_character_skin(soup: BeautifulSoup) -> Dict:
  """ 获取角色装扮和对应头像 """
  skinbox = soup.select('#skinbox > div')
  big_header_div, skin_div = skinbox[0], skinbox[1]

  # 获取装扮和对应的头像下载地址
  res = {}
  i = 0
  for skin_header in big_header_div.select('div'):
    skin_name = skin_header.select_one('h3').text
    header_url = f"{BASE_URL}{skin_header.select_one('img').get('src')}"
    skin_url = f"{BASE_URL}{skin_div.contents[i].select_one('a').get('href')}"

    header_file_name = f"{skin_name}_头像.{header_url.split('.')[-1]}"
    skin_file_name = f"{skin_name}.{skin_url.split('.')[-1]}"

    res[header_file_name] = header_url
    res[skin_file_name] = skin_url
    i += 1

  return res


def get_character_emoji(soup: BeautifulSoup) -> Dict:
  """ 获取角色表情包 """
  emoji_div = soup.select_one('#character_emoji').find_next_sibling('div').select('img')
  res = {}
  for emoji in emoji_div:
    emoji_url = f"{BASE_URL}{emoji.get('src')}"
    file_name = emoji_url.split('/')[-1]
    res[file_name] = emoji_url

  return res


def get_character_voice(soup: BeautifulSoup) -> Dict:
  """ 获取角色语音包 """
  soundbox = soup.select('#soundbox > div')
  sound_res = {}
  actor_line_res = []
  for sound in soundbox:
    triggrt = sound.select_one('h3').text.replace(' ', '')
    audio = f"{BASE_URL}{sound.select_one('button').get('data-src')}"
    actor_line = sound.select('p')
    actor_line_zh = actor_line[0].text
    actor_line_jp = actor_line[1].text
    file_name = f"{triggrt.replace('/','')}.{audio.split('.')[-1]}"
    sound_res[file_name] = audio
    actor_line_res.append((triggrt, actor_line_zh, actor_line_jp))
  res = {
      'sound': sound_res,
      'actor_line': actor_line_res,
  }
  return res


def get_character_info(url: str, name: str = None) -> dict:
  character_page = new_beautiful_soup(url)
  h1 = character_page.select_one('#character_intro')
  # name = h1.text

  # 获取雀士基本信息
  char_info = []
  base_info = h1.find_next('div')
  for info in base_info.select('p'):
    char_info.append(info.select('span')[1].text)
# 获取雀士介绍
  story = base_info.find_next_sibling('p').text
  # 获取雀士立绘
  skin = get_character_skin(character_page)
  # 获取雀士表情包
  emoji = get_character_emoji(character_page)
  # 获取雀士台词和语音
  voice = get_character_voice(character_page)
  # TODO 保存雀士台词

  result = {
      'info': char_info,
      'story': story,
      'skin': skin,
      'emoji': emoji,
      'voice': voice['sound'],
      'actor_line': voice['actor_line'],
  }
  return result


def download():
  """ 下载装扮，表情包，语音 """
  with open(os.path.join(BASE_DIR, RES_JSON), 'r', encoding='utf-8') as resf:
    res = json.load(resf)
  for name, urls in res.items():
    path = new_save_dir(name)

    relist = {}
    for file_name, url in urls.items():
      # 存在则跳过，避免重复下载
      fpath = os.path.join(path, file_name)
      if os.path.exists(fpath) and os.path.getsize(fpath) > 0:
        continue

      try:
        r = requests.get(url)
      except requests.exceptions.ConnectionError:
        relist[file_name] = url
        continue
      with open(fpath, 'wb') as f:
        f.write(r.content)
      print(f"download done....{file_name:120}")
    res[name] = relist
    print(f"download done....{name:120}")

  res = {k: v for k, v in res.items() if len(v) > 0}

  if len(res) > 0:
    with open(os.path.join(BASE_DIR, RES_JSON), 'w', encoding='utf-8') as f:
      json.dump(res, f, ensure_ascii=False, indent=2)
    download()
  else:
    with open(os.path.join(BASE_DIR, RES_JSON), 'w', encoding='utf-8') as f:
      f.write('{}')


def start(names: [str] = None):
  download_list = {}
  characters = []

  character_list = new_beautiful_soup(CHARACTER_PAGE)
  max = len(character_list.select('#main > div > div > ul > li'))
  i = 1
  for character in character_list.select('#main > div > div > ul > li'):
    a = character.select_one('a')
    name = a.get('title')
    if names and name not in names:
      continue
    url = a.get('href')
    character_info = get_character_info(url)
    dirs = new_save_dir(name)
    # 文字信息优先保存到 csv 文件中
    # 台词
    with open(os.path.join(dirs, f"{name}actor_line.csv"), 'w', encoding='utf-8') as f:
      writer = csv.writer(f)
      writer.writerow(['触发条件', '中文台词', '日文台词'])
      writer.writerows(character_info['actor_line'])
    # 角色介绍
    with open(os.path.join(dirs, f"{name}_store.txt"), 'w', encoding='utf-8') as f:
      f.write(character_info['story'])

    download_list[name] = {**character_info['skin'], **character_info['emoji'], **character_info['voice']}
    characters.append(character_info['info'])
    print(f"\r[{i:02}/{max}] {name:120} done", end='', flush=True)
    i += 1
  print()
  with open(os.path.join(BASE_DIR, RES_JSON), 'w', encoding='utf-8') as f:
    json.dump(download_list, f, ensure_ascii=False, indent=2)
  with open(os.path.join(BASE_DIR, 'characters.csv'), 'w', encoding='utf-8') as f:
    writer = csv.writer(f)
    writer.writerow(['姓名', '身高', '生日', '年龄', '血型', 'CV', '兴趣'])
    writer.writerows(characters)
  download()


if __name__ == '__main__':
  start()
