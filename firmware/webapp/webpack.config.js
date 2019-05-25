const path = require('path');
const ExtractTextPlugin = require('extract-text-webpack-plugin');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const ScriptExtHtmlWebpackPlugin = require('script-ext-html-webpack-plugin');
const webpack = require('webpack');

const plugins = [
  new HtmlWebpackPlugin({
    template: './src/index.html',
    favicon: 'favicon.ico'
  }),
  new ScriptExtHtmlWebpackPlugin({
    defaultAttribute: 'defer',
  }),
  new ExtractTextPlugin({
    filename: './[name].[hash].css',
    allChunks: true,
  }),
  new webpack.optimize.ModuleConcatenationPlugin(),
];

module.exports = () => ({
  entry: [
    './src/index.js',
    './styles/app.css',
    './styles/side-menu.css',
    './styles/loader.css',
    './node_modules/purecss/build/pure.css'
  ],
  devtool: 'source-map',
  output: {
    filename: '[name].[hash].js',
    path: path.resolve(__dirname, './'),
  },
  module: {
    rules: [
      {
        test: /\.js$/,
        loader: 'babel-loader',
        include: [
          path.resolve(__dirname, './'),
        ],
      },
      {
        test: /\.css$/,
        use: ExtractTextPlugin.extract({
          use: 'css-loader?importLoaders=1',
        }),
      },
    ],
  },
  plugins,
  devServer: {
    contentBase: path.resolve('static'),
    publicPath: '/',
    open: false,
  },
});
