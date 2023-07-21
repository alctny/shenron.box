package db

import (
	"ahblog/dao"
	"os"

	"gorm.io/driver/mysql"
	"gorm.io/gorm"
	"gorm.io/gorm/schema"
)

const (
	dsn = "root:ahblogsql@tcp(127.0.0.1:3306)/ahblog"
)

func iferr(err error) {
	if err != nil {
		println(err)
		os.Exit(1)
	}
}

func DBTest() {
	db, err := gorm.Open(mysql.Open(dsn), &gorm.Config{
		NamingStrategy: schema.NamingStrategy{SingularTable: true},
	})
	iferr(err)

	t := dao.Tag{
		Visibleness: 1,
		TagName:     "os",
	}
	err = db.Create(&t).Error
	iferr(err)
}
