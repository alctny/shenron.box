package dao

// Tag 標籤
type Tag struct {
	Visibleness int    `gorm:"visiblness"` // 擁有此標籤的博文的可見性
	TagName     string `gorm:"tag_name"`   // 標籤名稱
}

// func (Tag) TableName() string {
// 	return "tag"
// }
