<!DOCTYPE html>
<html lang="ja">
    <head>
        <meta charset="UTF-8">
        <!--<title>ESP32_HIEYUKARISAN</title>-->
    </head> 
<body>
    <img src="media/image/PXL_20220318_112853861.MP.jpg">
    <h1>ひえゆかりさん制作</h1>
    <h2>0.妄想</h2>
    <h3>(1)冷蔵庫の中に置いておき、扉を開けると明るさ変化を検知する。</h3>
    <h3>(2)時刻は無線LAN経由でNTPで取得する。</h3>
    <H3>(3)時間帯ごとに、ランダムなMP3ファイルを再生する。</H3>
    <h3>(4)明るさが設定値を超えた状態が設定時間以上になると、警告する。</h3>
    <h2>1.持たせたい機能</h2>
    <h2>2.SDカード内ディレクトリ構造</h2>
        <div style="margin-left: 10px;">
            <table>
                <tr>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td colspan="12">音声</td>
                </tr>
                <tr>
                    <td>カレント</td>
                    <td>フォルダ</td>
                    <td>ファイル形式</td>
                    <td>定義</td>
                    <td>１</td>
                    <td>２</td>
                    <td>3</td>
                    <td>4</td>
                    <td>5</td>
                    <td>6</td>
                    <td>7</td>
                    <td>8</td>
                    <td>9</td>
                    <td>10</td>
                    <td>11</td>
                </tr>
                <tr>
                    <td>root(SD:)</td>
                    <td>00</td>
                    <td>mp3</td>
                    <td>0時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>01</td>
                    <td>mp3</td>
                    <td>1時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>02</td>
                    <td>mp3</td>
                    <td>2時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>03</td>
                    <td>mp3</td>
                    <td>3時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>04</td>
                    <td>mp3</td>
                    <td>4時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>05</td>
                    <td>mp3</td>
                    <td>5時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>06</td>
                    <td>mp3</td>
                    <td>6時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>07</td>
                    <td>mp3</td>
                    <td>7時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>08</td>
                    <td>mp3</td>
                    <td>8時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>09</td>
                    <td>mp3</td>
                    <td>9時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>10</td>
                    <td>mp3</td>
                    <td>10時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>11</td>
                    <td>mp3</td>
                    <td>11時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>12</td>
                    <td>mp3</td>
                    <td>12時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>13</td>
                    <td>mp3</td>
                    <td>13時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>14</td>
                    <td>mp3</td>
                    <td>14時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>15</td>
                    <td>mp3</td>
                    <td>15時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>16</td>
                    <td>mp3</td>
                    <td>16時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>17</td>
                    <td>mp3</td>
                    <td>17時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>18</td>
                    <td>mp3</td>
                    <td>18時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>19</td>
                    <td>mp3</td>
                    <td>19時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>20</td>
                    <td>mp3</td>
                    <td>20時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>21</td>
                    <td>mp3</td>
                    <td>21時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>22</td>
                    <td>mp3</td>
                    <td>22時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>23</td>
                    <td>mp3</td>
                    <td>23時台</td>
                </tr>
                <tr>
                    <td></td>
                    <td>90</td>
                    <td>mp3</td>
                    <td>その他</td>
                    <td>冷蔵庫、開けっ放しじゃないですか？</td>
                </tr>
            </table>
        </div>
    <h2>3.原価</h2>
    <div style="margin-left: 10px;">
        <table style="border: 2px; border-color: orange;">        
            <tr>
                <td>品名</td>
                <td>単価</td>
                <td>調達先</td>
                <td>備考</td>
            </tr>
            <tr>
                <td>esp32</td>
                <td>581</td>
                <td>Aliexpress</td>
                <td>送料込み</td>
            </tr>
            <tr>
                <td>dfplayermini</td>
                <td>227</td>
                <td>Aliexpress</td>
                <td>454/2 送料込み</td>
            </tr>
            <tr>
                <td>tfカードリーダ</td>
                <td>101</td>
                <td>Aliexpress</td>
                <td>送料込み</td>
            </tr>
            <tr>
                <td>DC-DCコンバータ</td>
                <td>122</td>
                <td>Aliexpress</td>
                <td>送料込み</td>
            </tr>
            <tr>
                <td>マイクロスピーカーユニット</td>
                <td>198</td>
                <td>Aliexpress</td>
                <td>594/3 送料込み</td>
            </tr>
            <tr>
                <td><s>ZLコネクタ 2P（基板・電線側セット）</s></td>
                <td><s>20</s></td>
                <td><s>マルツ</s></td>
                <td></td>
            </tr>
            <tr>
                <td><s>ZLコネクタ 4P（基板・電線側セット）</s></td>
                <td><s>20</s></td>
                <td><s>マルツ</s></td>
                <td></td>
            </tr>
            <tr>
                <td>UL電線 AWG30</td>
                <td>309</td>
                <td>マルツ</td>
                <td></td>
            </tr>
            <tr>
                <td>熱収縮チューブ 20mm</td>
                <td>150</td>
                <td>マルツ</td>
                <td></td>
            </tr>
            <tr>
                <td>熱収縮チューブ 25mm</td>
                <td>165</td>
                <td>マルツ</td>
                <td></td>
            </tr>
            <tr>
                <td>白カラスペーサ</td>
                <td>13.2(3.3*4)</td>
                <td>マルツ</td>
                <td>C-305 165 50個 1SPあたり4個</td>
            </tr>
            <tr>
                <td>ユニバーサル基板</td>
                <td>120</td>
                <td>マルツ</td>
                <td>タカチ TNF11-16</td>
            </tr>
            <tr>
                <td><s>ユニバーサル基板<s></td>
                <td><s>105</s></td>
                <td><s>マルツ</s></td>
                <td><s>タカチ　TNF12-25</s></td>
            </tr>
            <tr>
                <td>A4下敷き</td>
                <td>110</td>
                <td>ダイソー</td>
                <td>スピーカーガード用</td>
            </tr>
            <tr>
                <td>ナベ小ねじM2×10</td>
                <td>40(9.8*4)</td>
                <td>ムサシ</td>
                <td>ヤハタ</td>
            </tr>
            <tr>
                <td>六角ボルトM12×25</td>
                <td>60</td>
                <td>ムサシ</td>
                <td>ヤハタ</td>
            </tr>
            <tr>
                <td>9V角電池</td>
                <td>110</td>
                <td>ダイソー</td>
                <td></td>
            </tr>
            <tr>
                <td>XHコネクタ（候補）</td>
                <td></td>
                <td></td>
                <td>https://www.jst-mfg.com/product/pdf/jpn/XH.pdf?236dc0b5906d</td>
            </tr>
            <tr>
                <td>フォトトランジスタ</td>
                <td>50</td>
                <td>マルツ</td>
                <td>NJL 7502L https://akizukidenshi.com/catalog/g/gI-02325/</td>
            </tr>
            <tr>
                <td>【P-12731】両面スルーホールガラスコンポジット・ユニバーサル基板　Ｆタイプ</td>
                <td>15</td>
                <td>秋月</td>
                <td>/</td>
            </tr>
            <tr>
                <td>【C-12263】ＸＨコネクタ　ベース付ポスト　サイド型　３Ｐ　Ｓ３Ｂ－ＸＨ－Ａ（ＬＦ）（ＳＮ）</td>
                <td>10</td>
                <td>秋月</td>
                <td></td>
            </tr>
            <tr>
                <td>【C-12256】ＸＨコネクタ　ハウジング　３Ｐ　ＸＨＰ－３</td>
                <td>5</td>
                <td>秋月</td>
                <td></td>
            </tr>
            <tr>
                <td>【C-12265】ＸＨコネクタ　ハウジング用コンタクト　ＳＸＨ－００１Ｔ－Ｐ０．６　（１００個入）</td>
                <td>14</td>
                <td>秋月</td>
                <td></td>
            </tr>
            <tr>
                <td>【C-12262】ＸＨコネクタ　ベース付ポスト　サイド型　２Ｐ　Ｓ２Ｂ－ＸＨ－Ａ（ＬＦ）（ＳＮ）</td>
                <td>20</td>
                <td>秋月</td>
                <td></td>
            </tr>
            <tr>
                <td>【C-12256】ＸＨコネクタ　ハウジング　３Ｐ　ＸＨＰ－３</td>
                <td>5</td>
                <td>秋月</td>
                <td></td>
            </tr>
            <tr>
                <td>【C-12255】ＸＨコネクタ　ハウジング　２Ｐ　ＸＨＰ－２</td>
                <td>10</td>
                <td>秋月</td>
                <td></td>
            </tr>
            <tr>
                <td>合計</td>
                <td>2441</td>
                <td></td>
                <td></td>
            </tr>
        </table>
    </div>
</body>
</html>

