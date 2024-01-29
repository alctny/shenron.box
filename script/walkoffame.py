#! /usr/bin/env python

# 获取星光大道留名的明星列表
# https://walkoffame.com/browse-stars

import requests
from bs4 import BeautifulSoup
import csv
import json

HOME_URL = 'https://walkoffame.com/browse-stars'


def new_beautiful_soup(url: str, trymax: int = 3) -> BeautifulSoup:
  header = {'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) \
Chrome/120.0.0.0 Safari/537.36'}
  for count in range(trymax):
    try:
      r = requests.get(url, headers=header)
      return BeautifulSoup(r.text, 'html.parser')
    except Exception as e:
      if count == trymax - 1:
        raise e


def start():
  start_list = new_beautiful_soup(HOME_URL)
  data = []
  json_data = {}
  i = 0
  max = len(start_list.select(".letter-section li > a"))
  for a in start_list.select(".letter-section li > a"):
    i += 1
    name = a.text.strip()
    href = a.get("href")
    json_data[name] = href
    detail_pate = new_beautiful_soup(href)
    base_info = detail_pate.select_one("li.honoree-star")
    if base_info:
      bi = {}
      for p in base_info.select("p"):
        title = p.select_one("strong").text.strip()
        content = p.select_one("span").text.strip()
        bi[title] = content
      data.append([name, bi.get('Category', ''), bi.get('Address', ''), bi.get('Ceremony date', ''), href])
      # 设置 end 为空字符串避免因为上一行比本行长而导致显式错乱
      print(f"\r [{i}/{max}] {name}: {bi}", end=f"{' ':30}", flush=True)
    else:
      data.append([[name, "", "", "", href]])
      print(f"\r [{i}/{max}] {name}: {href}", end=f"{' ':30}", flush=True)

  with open('walkoffame.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerows(data)
  with open('walkoffame.json', 'w') as f:
    json.dump(json_data, f)


if __name__ == '__main__':
  start()
