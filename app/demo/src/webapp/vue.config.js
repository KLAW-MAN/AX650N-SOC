const webpack = require('webpack')
module.exports = {
  productionSourceMap: false,
  lintOnSave: false,
  outputDir: "../../bin/frtdemo/web",
  configureWebpack: {
    plugins: [
      new webpack.NormalModuleReplacementPlugin(/element-ui[\/\\]lib[\/\\]locale[\/\\]lang[\/\\]zh-CN/, 'element-ui/lib/locale/lang/en')
    ]
  }
}