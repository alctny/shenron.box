// Note: 此高德天气查询 api 仅用于 polybar 中
// 因此不需要且不应该做复杂的错误处理

package main

import (
	"fmt"

	"github.com/go-resty/resty/v2"
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

func weather() (*Data, error) {
	client := resty.New()
	var response Data
	_, err := client.R().SetResult(&response).Get(api)
	return &response, err

	// fmt.Println()
}

func main() {
	weather, err := weather()
	if err != nil {
		fmt.Println(err)
		return
	}

	if weather.Status != "1" || len(weather.Lives) < 1 {
		fmt.Println("no weather info")
		return
	}
	local := weather.Lives[0]
	wstr := fmt.Sprintf("%s %s %s°C", local.City, local.Weather, local.Temperature_float)
	fmt.Println(wstr)
}
