package dao

// Group 分組
type Group struct {
	Visibleness int     // 可見性
	GroupName   string  // 分組名稱
	PreGroup    *Group  // 父級分組
	DefaultTag  []Tag   // 屬於此分組的博文默認擁有的標籤
	NxtGroup    []Group // 子分組
}
