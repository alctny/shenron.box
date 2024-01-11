#! /usr/bin/env python
# -*- coding: utf-8 -*-

import requests
from bs4 import BeautifulSoup
from typing import List
from typing import Dict
import sys
import csv
import argparse

# 首页
INDEX = "https://www.stats.gov.cn/sj/tjbz/tjyqhdmhcxhfdm/2022/index.html"
# 输出文件路径
DATA_PATH = "adcode.csv"
# 出现网络错误时重试次数
TRY_MAX = 3
# 不同层级的选择器，从上往下分别是 省、市、县、乡、村
SELECTER = [
    "tr.provincetr",
    "tr.citytr",
    "tr.countytr",
    "tr.towntr",
    "tr.villagetr",
]


def new_beautiful_soup(url: str) -> BeautifulSoup:
  for count in range(TRY_MAX):
    try:
      r = requests.get(url)
      r.encoding = 'utf-8'
      return BeautifulSoup(r.text, 'html.parser')
    except Exception as e:
      if count == TRY_MAX - 1:
        raise e


def just_urls(url: str) -> Dict:
  ''' 仅仅获取下级区域的访问地址，而不获取区域编码 '''
  content = new_beautiful_soup(url)

  for selecter in SELECTER:
    if content.select_one(selecter):
      break
  if selecter == "tr.villagetr":
    return None

  urls = {}
  if selecter == "tr.provincetr":
    for prov in content.select("tr.provincetr a"):
      url = "/".join(url.split("/")[:-1]) + "/" + prov.get("href")
      prov_name = prov.text
      urls[prov_name] = url
  else:
    for cct in content.select(selecter):
      td = cct.select("td")
      a = td[0].select_one("a")
      if not a:
        # 有些地区没有下级行政区域，比如浙江省>宁波市>市辖区
        continue
      href = a.get("href")
      url = "/".join(url.split("/")[:-1]) + "/" + href
      cct_name = td[1].text
      urls[cct_name] = url
  return urls


def get_adcode(url: str) -> List[List[str]]:
  ''' 爬取地理编码，但不填充经纬度 '''

  data = []
  content = new_beautiful_soup(url)
  for selecter in SELECTER:
    if content.select_one(selecter):
      break

  if selecter == "tr.provincetr":
    ''' 对于省级，只需要获取下级网址，且不递归毫无意义 '''
    for prov in content.select(f"{selecter} a"):
      href = prov.get("href")
      url = "/".join(url.split("/")[:-1]) + "/" + href
      get_adcode(url, True)

  elif selecter == "tr.villagetr":
    ''' 对于村级，需要获取地理编码 '''
    for vill in content.select(selecter):
      td = vill.select("td")
      adcode, address = td[0].text, td[2].text
      data.append([address, adcode])

  elif selecter in ["tr.citytr", "tr.countytr", "tr.towntr"]:
    ''' 对于市、县、乡级，需要获取下级网址，且递归 '''
    for cct in content.select(selecter):
      td = cct.select("td")
      adcode, address = td[0].text, td[1].text
      data.append([address, adcode])
      a = td[0].select_one("a")
      if not a:
        print(f"\r{address:40}\tOK", end="", flush=True)
        continue
      href = td[0].select_one("a").get("href")
      nextLvUrl = "/".join(url.split("/")[:-1]) + "/" + href
      data += get_adcode(nextLvUrl)
      print(f"\r{address:40}\tOK", end="", flush=True)

  else:
    raise Exception("无法解析网页，有可能是网页结构发生变动，请联系开发者")

  return data


def adcode_to_csv(url: str, sigle: bool):
  if sigle:
    ''' 保存为单个文件 '''
    data = get_adcode(url)
    with open(DATA_PATH, "w", encoding="utf-8") as f:
      csv.writer(f).writerows(data)
  else:
    ''' 保存为多个文件 '''
    urls = just_urls(url)
    for k, v in urls.items():
      data = get_adcode(v)
      with open(f"{k}.csv", "w", encoding="utf-8") as f:
        csv.writer(f).writerows(data)


def usage():
  print("\033[31mUsage: python adcode.py [url] [sigle]\033[0m")
  print("\033[31murl: 地理编码首页地址\033[0m")
  print("\033[31msigle: 是否保存为单个文件，可选值为 True/False，默认为 False(请注意大小写)\033[0m")


# 仅用于测试
test_url = [
    "https://www.stats.gov.cn/sj/tjbz/tjyqhdmhcxhfdm/2022/index.html",
    "https://www.stats.gov.cn/sj/tjbz/tjyqhdmhcxhfdm/2022/51.html",
    "https://www.stats.gov.cn/sj/tjbz/tjyqhdmhcxhfdm/2022/51/5101.html",
    "https://www.stats.gov.cn/sj/tjbz/tjyqhdmhcxhfdm/2022/51/01/510104.html",
    "https://www.stats.gov.cn/sj/tjbz/tjyqhdmhcxhfdm/2022/51/01/04/510104019.html",
]


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("-u", "--url", help="地理编码首页地址")
  parser.add_argument("-s", "--sigle", help="是否保存为单个文件，可选值为 True/False，默认为 False(请注意大小写)")

  try:
    args = parser.parse_args()
  except Exception as e:
    print(e)
    sys.exit(1)
  adcode_to_csv(args.url, args.sigle)
