// Note: 此高德天气查询 api 仅用于 polybar 中
// 因此不需要且不应该做复杂的错误处理

package main

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
)

const (
	key   = "6f1cec4d0a55e8b59afe70567327661b"
	_api  = "https://restapi.amap.com/v3/weather/weatherInfo?key=%s&city=%s"
	wuhou = "510107"
)

var api string

func init() {
	api = fmt.Sprintf(_api, key, wuhou)
}

type (
	Live struct {
		// Province          string `json:"province"`
		City string `json:"city"`
		// Adcode            string `json:"adcode"`
		Weather string `json:"weather"`
		// Temperature       string `json:"temperature"`
		// Winddirection     string `json:"winddirection"`
		// Windpower         string `json:"windpower"`
		// Humidity          string `json:"humidity"`
		// Reporttime        string `json:"reporttime"`
		Temperature_float string `json:"temperature_float"`
		// Humidity_float    string `json:"humidity_float"`
	}

	Data struct {
		Status string `json:"status"`
		// Count    string `json:"count"`
		// Info     string `json:"info"`
		// Infocode string `json:"infocode"`
		Lives []Live `json:"lives"`
	}
)

func exit() {
	fmt.Println("have a good day")
	os.Exit(1)
}

func ckErr(err error) {
	if err != nil {
		exit()
	}
}

func weather() {
	resp, err := http.Get(api)
	ckErr(err)
	if resp.StatusCode != http.StatusOK {
		exit()
	}

	body, err := io.ReadAll(resp.Body)
	ckErr(err)
	var weatherInfo Data
	err = json.Unmarshal(body, &weatherInfo)
	ckErr(err)

	if weatherInfo.Status != "1" || len(weatherInfo.Lives) < 1 {
		exit()
	}
	local := weatherInfo.Lives[0]
	fmt.Println(local.City, local.Weather, local.Temperature_float+"°C")
}

func main() {
	weather()
}
