#include <linux/module.h>
#include <linux/bottom_half.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/cache.h>
#include <linux/jhash.h>
#include <linux/init.h>
#include <linux/times.h>

#include <net/net_namespace.h>
#include <net/icmp.h>
#include <net/inet_hashtables.h>
#include <net/tcp.h>
#include <net/transp_v6.h>
#include <net/ipv6.h>
#include <net/inet_common.h>
#include <net/timewait_sock.h>
#include <net/xfrm.h>
#include <net/netdma.h>
#include <net/secure_seq.h>

#include <linux/inet.h>
#include <linux/ipv6.h>
#include <linux/stddef.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <linux/crypto.h>
#include <linux/scatterlist.h>


static void get_tcp4_listen_sock(struct sock *sk, struct seq_file *f, int i, int *len)
{
	u8 max_qlen_log;
	int qlen, qlen_young;
	const struct inet_connection_sock *icsk = inet_csk(sk);
	struct inet_sock *inet = inet_sk(sk);
	__be32 dest = inet->daddr;
	__be32 src = inet->rcv_saddr;
	__u16 destp = ntohs(inet->dport);
	__u16 srcp = ntohs(inet->sport);

	if(icsk && icsk->icsk_accept_queue.listen_opt) {
		max_qlen_log = icsk->icsk_accept_queue.listen_opt->max_qlen_log;
		qlen = icsk->icsk_accept_queue.listen_opt->qlen;
		qlen_young = icsk->icsk_accept_queue.listen_opt->qlen_young;
	}

	/*   sl local_address  ack_backlog max_ack_backlog qlen_young qlen max_qlen uid inode sock*/
	seq_printf(f, "%4d: "NIPQUAD_FMT":%u "NIPQUAD_FMT":%u %u %u %u %u %u %u %lu %p%n",
		i, 
		NIPQUAD(src),
		srcp,
		NIPQUAD(dest),
		destp,
		sk->sk_ack_backlog,
		sk->sk_max_ack_backlog,
		qlen_young,
		qlen,
		1 << max_qlen_log,
		sock_i_uid(sk),
		sock_i_ino(sk),
		sk,
		len);
}

#define TMPSZ 150

static int tcp4_listen_seq_show(struct seq_file *seq, void *v)
{
	struct tcp_iter_state *st;
	int len = 0;

	if (v == SEQ_START_TOKEN) {
		seq_printf(seq, "%-*s\n", TMPSZ - 1,
			   "  sl local_address remote_address ack_backlog max_ack_backlog qlen_young qlen max_qlen uid inode sock");
		goto out;
	}
	st = seq->private;

	if (TCP_SEQ_STATE_LISTENING == st->state) {
		get_tcp4_listen_sock(v, seq, st->num, &len);
		seq_printf(seq, "%*s\n", TMPSZ - 1 - len, "");
	}
out:
	return 0;
}


static struct tcp_seq_afinfo tcp4_listen_seq_afinfo = {
	.name		= "tcp_listen",
	.family		= AF_INET,
	.seq_fops	= {
		.owner		= THIS_MODULE,
	},
	.seq_ops	= {
		.show		= tcp4_listen_seq_show,
	},
};

static int __init tcp_listen_stat_init(void)
{
	return tcp_proc_register(&init_net, &tcp4_listen_seq_afinfo);
}

static void __exit tcp_listen_stat_exit(void)
{
	tcp_proc_unregister(&init_net, &tcp4_listen_seq_afinfo);
}

module_init(tcp_listen_stat_init);
module_exit(tcp_listen_stat_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("listen stat: dump all listen state tcp4 socket in detail");
MODULE_AUTHOR("sTeeL <steel.mental@gmail.com>");
