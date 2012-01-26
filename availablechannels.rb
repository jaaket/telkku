require 'nokogiri'
require 'open-uri'

(1..118).each do |i|
	doc = Nokogiri::HTML(open("http://m.telkku.com/channel/show/#{i}/20120126"))

	doc.css('caption').each do |caption|
		print '"' + caption.content + '", '
	end

	if i % 5 == 0
		print "\n"
	end
end