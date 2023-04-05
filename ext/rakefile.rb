
task :default do
	ruby_root = ENV['RUBY_ROOT']
	install_prefix = ENV['RUBYARCHDIR']
	
	"RUBYARCHDIR"=>"/home/samuel/.gem/ruby/3.2.1/extensions/x86_64-linux/3.2.0-static/protocol-quic-0.0.0",
	"RUBYLIBDIR"=>"/home/samuel/.gem/ruby/3.2.1/extensions/x86_64-linux/3.2.0-static/protocol-quic-0.0.0"}
 
	
	$stderr.puts ENV.inspect
	exit!(1)
end
