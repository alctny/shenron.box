package main

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

func main() {
	r := gin.Default()
	r.GET("/task", func(ctx *gin.Context) {
		ctx.JSON(http.StatusOK, gin.H{"msg": "ok", "task": "淦你大爷，今天不写代码"})
	})
	r.Run(":50001")
}
