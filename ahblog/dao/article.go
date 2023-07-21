package dao

// Article 博文
type Article struct {
	// 博文内容
	ArticleID string // 唯一標識
	Cover     string // 封面
	Tiele     string // 標題
	SubTiele  string // 副標題
	Abstract  string // 摘要
	HTMLRaw   string // HTML 父文本
	MDRaw     string // Markdown 原文
	CreateAt  int64  // 編寫日期
	UpdateAt  int64  // 修改時間
	// 数据和博文属性
	View        int32 // 訪問量
	Feedback    int32 // 反饋
	ViewOver    int32 // 完閱量
	Mutability  int32 // 可變性
	Visibleness int32 // 可見性
	// 关系网
	Reference []Reference // 外部连接
	Local     []Moments   // 朋友圈
	Tag       []Tag       // 標籤
}
