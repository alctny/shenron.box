package server

import (
	"github.com/gin-gonic/gin"
)

var r *gin.Engine

func init() {
	r = gin.Default()
	r.NoRoute(notFound)

	// 博文
	article := r.Group("article")
	article.GET("/", articleList)                  // 獲取博文列表
	article.PUT("/", updateArticle)                // 修改博文
	article.POST("/", addArticle)                  // 新增博文
	article.GET("/{article_id}", takeArticle)      // 獲取單篇博文詳情
	article.DELETE("/{article_id}", deleteArticle) // 刪除單篇博文

	// r.Group("group")

	// 文章标签
	tag := r.Group("tag")
	tag.GET("/", tagList)              // 获取标签列表
	tag.DELETE("/{tag_id}", deleteTag) // 删除标签
	tag.POST("/", addTag)              // 新增标签
	tag.PUT("/rename", renameTag)      // 标签从命名
	tag.PUT("/profile", setTag)        // 修改标签属性

}

func ServerStart(conf any) {
	r.Run()
}
